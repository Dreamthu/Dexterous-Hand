"""Command-line and ROS adapters for fixed-camera ArUco calibration."""

from __future__ import annotations

import argparse
from collections import deque
import copy
import hashlib
import importlib
import json
import os
from pathlib import Path
import shlex
import shutil
import sys
import time
from typing import Any, Mapping, Sequence

import cv2
import numpy as np

from .core import (
    CalibrationError,
    atomic_write_json,
    atomic_write_yaml,
    board_snapshot,
    camera_snapshot,
    compatible_camera,
    detect_board_pose,
    generate_board_png,
    load_config,
    load_json,
    load_yaml,
    invert_transform,
    matrix_from_pose,
    pose_dict,
    rounded_matrix,
    solve_eye_to_hand,
    training_quality,
    transform_from_dict,
    transform_errors,
    utc_now,
    validation_metrics,
    validation_quality,
)
from .safety import base_from_camera_root, robot_contract, stationary_window, validate_tool_snapshot


PROJECT_ROOT = Path(__file__).resolve().parents[2]
DEFAULT_CONFIG = PROJECT_ROOT / "config/eye_to_hand_aruco.yaml"


def repository_path(value: str | Path) -> Path:
    path = Path(value).expanduser()
    return path.resolve() if path.is_absolute() else (PROJECT_ROOT / path).resolve()


def portable_path(path: Path) -> str:
    """Prefer a project-relative path without rejecting deliberate external outputs."""
    try:
        return str(path.relative_to(PROJECT_ROOT))
    except ValueError:
        return str(path)


def dataset_path(config: Mapping[str, Any], dataset_kind: str) -> Path:
    key = "calibration_dataset" if dataset_kind == "calibration" else "validation_dataset"
    return repository_path(config["capture"][key])


def result_path(config: Mapping[str, Any]) -> Path:
    return repository_path(config["solve"]["result"])


def stamp_seconds(stamp: Any) -> float:
    return float(stamp.sec) + float(stamp.nanosec) * 1e-9


def stamp_text(stamp: Any) -> str:
    return f"{int(stamp.sec)}.{int(stamp.nanosec):09d}"


def pose_message_transform(message: Any) -> np.ndarray:
    position = message.pose.position
    orientation = message.pose.orientation
    return matrix_from_pose(
        (position.x, position.y, position.z),
        (orientation.x, orientation.y, orientation.z, orientation.w),
    )


def _same_capture_contract(dataset: Mapping[str, Any], config: Mapping[str, Any], kind: str) -> None:
    if dataset.get("dataset_kind") != kind:
        raise CalibrationError(
            f"dataset kind is {dataset.get('dataset_kind')!r}, expected {kind!r}"
        )
    if dataset.get("calibration_type") != "eye_to_hand":
        raise CalibrationError("dataset is not eye-to-hand data")
    if dataset.get("board") != board_snapshot(config["board"]):
        raise CalibrationError("dataset board geometry differs from current configuration")
    expected_frames = {
        "base_frame": str(config["frames"]["base_frame"]),
        "camera_frame": str(config["frames"]["camera_frame"]),
        "robot_pose_child_frame": str(config["frames"]["robot_pose_child_frame"]),
    }
    if dataset.get("frames") != expected_frames:
        raise CalibrationError("dataset frame contract differs from current configuration")
    if "robot" in config:
        if dataset.get("robot_contract") != robot_contract(config):
            raise CalibrationError("dataset tool contract differs or is missing; use a new capture dataset")
        for sample in dataset.get("samples", []):
            if "tool_snapshot" not in sample:
                raise CalibrationError("dataset contains unmonitored legacy tool samples")
            validate_tool_snapshot(sample["tool_snapshot"], config["robot"])


def new_dataset(config: Mapping[str, Any], kind: str, camera: Mapping[str, Any]) -> dict[str, Any]:
    return {
        "schema_version": 1,
        "calibration_type": "eye_to_hand",
        "dataset_kind": kind,
        "created_at": utc_now(),
        "updated_at": utc_now(),
        "frames": {
            "base_frame": str(config["frames"]["base_frame"]),
            "camera_frame": str(config["frames"]["camera_frame"]),
            "robot_pose_child_frame": str(config["frames"]["robot_pose_child_frame"]),
        },
        "topics": dict(config["topics"]),
        "robot_contract": robot_contract(config),
        "board": board_snapshot(config["board"]),
        "camera_info": dict(camera),
        "samples": [],
    }


def load_compatible_dataset(
    path: Path, config: Mapping[str, Any], kind: str, camera: Mapping[str, Any] | None = None
) -> dict[str, Any]:
    dataset = load_json(path)
    _same_capture_contract(dataset, config, kind)
    if camera is not None and not compatible_camera(dataset["camera_info"], camera):
        raise CalibrationError(
            "CameraInfo changed since this dataset was created; use a new dataset or restore the profile"
        )
    return dataset


def _camera_arrays(info: Any) -> tuple[np.ndarray, np.ndarray]:
    return np.asarray(info.k, dtype=np.float64).reshape(3, 3), np.asarray(
        info.d, dtype=np.float64
    )


def _put_lines(image: np.ndarray, lines: Sequence[str]) -> None:
    y = 28
    for line in lines:
        cv2.putText(image, line, (12, y), cv2.FONT_HERSHEY_SIMPLEX, 0.58, (0, 0, 0), 3)
        cv2.putText(image, line, (12, y), cv2.FONT_HERSHEY_SIMPLEX, 0.58, (80, 255, 80), 1)
        y += 24


def collect(config: Mapping[str, Any], kind: str) -> int:
    robot_contract(config)  # Legacy datasets are diagnosis-only, never append silently.
    from .tool_monitor import ToolMonitor
    try:
        import rclpy
        from cv_bridge import CvBridge
        from geometry_msgs.msg import PoseStamped
        from rclpy.node import Node
        from rclpy.qos import qos_profile_sensor_data
        from sensor_msgs.msg import CameraInfo, Image
    except ImportError as error:  # pragma: no cover - depends on sourced ROS environment
        raise CalibrationError(
            "ROS Python packages are unavailable; source ROS or configure an overlay, then run ./calibrate.sh"
        ) from error

    capture_config = config["capture"]
    frames = config["frames"]
    topics = config["topics"]
    output = dataset_path(config, kind)

    class CaptureNode(Node):
        def __init__(self) -> None:
            super().__init__(f"aruco_eye_to_hand_{kind}_collector")
            self.bridge = CvBridge()
            self.latest_image: Any | None = None
            self.latest_bgr: np.ndarray | None = None
            self.image_received_monotonic = 0.0
            self.latest_info: Any | None = None
            self.poses: deque[tuple[float, Any, np.ndarray]] = deque(maxlen=500)
            self.pose_advanced_monotonic = 0.0
            self.tool = ToolMonitor(self, config["robot"])
            self.create_subscription(
                Image, str(topics["color_image"]), self._image_callback, qos_profile_sensor_data
            )
            self.create_subscription(
                CameraInfo,
                str(topics["color_camera_info"]),
                self._info_callback,
                qos_profile_sensor_data,
            )
            self.create_subscription(
                PoseStamped, str(topics["robot_pose"]), self._pose_callback, qos_profile_sensor_data
            )

        def _image_callback(self, message: Any) -> None:
            try:
                image = self.bridge.imgmsg_to_cv2(message, desired_encoding="bgr8")
            except Exception as error:
                self.get_logger().error(f"failed to convert color image: {error}")
                return
            self.latest_image = message
            self.latest_bgr = image
            self.image_received_monotonic = time.monotonic()

        def _info_callback(self, message: Any) -> None:
            self.latest_info = message

        def _pose_callback(self, message: Any) -> None:
            try:
                transform = pose_message_transform(message)
            except CalibrationError as error:
                self.get_logger().error(f"invalid robot pose: {error}")
                return
            timestamp = stamp_seconds(message.header.stamp)
            if timestamp <= 0:
                self.poses.clear()
                return
            if self.poses and timestamp <= self.poses[-1][0]:
                # Clear history on duplicates/backwards time; repeated messages
                # cannot count as proof of continuous stationary observations.
                self.poses.clear()
                return
            self.poses.append((timestamp, message, transform))
            self.pose_advanced_monotonic = time.monotonic()

        def nearest_pose(self, image_time: float):
            if not self.poses:
                return None
            return min(self.poses, key=lambda entry: abs(entry[0] - image_time))

        def stationarity(self, image_time: float, paired_pose_time: float | None):
            return stationary_window(
                [(t, str(msg.header.frame_id), transform) for t, msg, transform in self.poses],
                image_time, str(frames["base_frame"]), capture_config, paired_pose_time,
            )

    rclpy.init(args=None)
    node = None
    window_name = f"ArUco eye-to-hand: {kind}"
    last_key: tuple[int, int] | None = None
    detection = None
    preview: np.ndarray | None = None
    try:
        node = CaptureNode()
        cv2.namedWindow(window_name, cv2.WINDOW_NORMAL)
        print("采集窗口按键：SPACE 保存当前静止姿态，Q/ESC 结束。工具不会发送机械臂命令。")
        while rclpy.ok():
            rclpy.spin_once(node, timeout_sec=0.02)
            # PnP/rendering may be slower than pose publication. Drain queued
            # callbacks before pairing observations rather than falling behind.
            for _ in range(32):
                rclpy.spin_once(node, timeout_sec=0.0)
            node.tool.poll()
            try:
                tool_snapshot = node.tool.require_current()
                tool_error = ""
            except CalibrationError as error:
                tool_snapshot = None
                tool_error = str(error)
                # Observations spanning a tool-service outage are not a valid
                # stationary window even if the pose numbers did not move.
                node.poses.clear()
            if node.latest_bgr is None or node.latest_image is None or node.latest_info is None:
                canvas = np.zeros((360, 640, 3), dtype=np.uint8)
                _put_lines(canvas, ["Waiting for image, CameraInfo and robot pose..."])
                cv2.imshow(window_name, canvas)
                key = cv2.waitKey(20) & 0xFF
                if key in (ord("q"), 27):
                    break
                continue

            image_message = node.latest_image
            image_key = (int(image_message.header.stamp.sec), int(image_message.header.stamp.nanosec))
            if image_key != last_key:
                last_key = image_key
                info = node.latest_info
                if int(info.width) != node.latest_bgr.shape[1] or int(info.height) != node.latest_bgr.shape[0]:
                    raise CalibrationError("color image dimensions do not match CameraInfo")
                if str(info.header.frame_id) != str(frames["camera_frame"]):
                    raise CalibrationError(
                        f"CameraInfo frame is {info.header.frame_id!r}, expected {frames['camera_frame']!r}"
                    )
                if str(image_message.header.frame_id) != str(frames["camera_frame"]):
                    raise CalibrationError("color Image frame differs from CameraInfo / configured optical frame")
                camera_matrix, distortion = _camera_arrays(info)
                detection = detect_board_pose(
                    node.latest_bgr,
                    camera_matrix,
                    distortion,
                    str(info.distortion_model),
                    config["board"],
                    int(capture_config["minimum_markers"]),
                )
                preview = node.latest_bgr.copy()
                if detection is not None:
                    cv2.aruco.drawDetectedMarkers(
                        preview,
                        list(detection.corners),
                        np.asarray(detection.marker_ids, dtype=np.int32).reshape(-1, 1),
                    )
                    cv2.drawFrameAxes(
                        preview,
                        camera_matrix,
                        distortion,
                        detection.rvec,
                        detection.tvec,
                        float(config["board"]["marker_length_m"]),
                    )
            assert preview is not None
            display = preview.copy()
            image_time = stamp_seconds(image_message.header.stamp)
            nearest = node.nearest_pose(image_time)
            stationary = node.stationarity(image_time, nearest[0] if nearest is not None else None)
            sync_delta = float("inf") if nearest is None else abs(nearest[0] - image_time)
            lines = [
                f"set={kind}  saved={len(load_json(output)['samples']) if output.is_file() else 0}",
                (
                    f"markers={len(detection.marker_ids)}  reproj={detection.reprojection_rms_px:.2f}px"
                    if detection is not None
                    else "board not detected / too few configured markers"
                ),
                f"pose sync={sync_delta * 1000:.0f}ms  stable={'YES' if stationary.passed else 'NO'} ({stationary.sample_count})",
                f"tool={'OK' if tool_snapshot is not None else 'BLOCKED'}  history={stationary.coverage_s:.2f}s",
                tool_error or stationary.reason,
                "SPACE=capture  Q/ESC=finish",
            ]
            _put_lines(display, lines)
            cv2.imshow(window_name, display)
            key = cv2.waitKey(20) & 0xFF
            if key in (ord("q"), 27):
                break
            if key != 32:
                continue

            try:
                tool_snapshot = node.tool.require_current()
            except CalibrationError as error:
                print("拒绝：工具配置检查失败：" + str(error), file=sys.stderr)
                continue
            ros_now = node.get_clock().now().nanoseconds * 1e-9
            if (time.monotonic() - node.image_received_monotonic > float(capture_config["maximum_image_age_s"])
                    or not -float(capture_config["maximum_sync_delta_s"]) <= ros_now - image_time <= float(capture_config["maximum_image_age_s"])):
                print("拒绝：图像过期或时间基准不一致；检查相机时间戳与 ROS 时钟。", file=sys.stderr)
                continue
            if detection is None:
                print("拒绝：未检测到足够的标定板 marker。", file=sys.stderr)
                continue
            if detection.reprojection_rms_px > float(capture_config["maximum_reprojection_rms_px"]):
                print("拒绝：ArUco 重投影误差过大。", file=sys.stderr)
                continue
            if nearest is None or sync_delta > float(capture_config["maximum_sync_delta_s"]):
                print("拒绝：图像与右臂位姿时间不同步。", file=sys.stderr)
                continue
            if (time.monotonic() - node.pose_advanced_monotonic > float(capture_config["maximum_pose_age_s"])
                    or not -float(capture_config["maximum_sync_delta_s"]) <= ros_now - nearest[0] <= float(capture_config["maximum_pose_age_s"])):
                print("拒绝：机器人位姿过期或时间基准不一致。", file=sys.stderr)
                continue
            if not stationary.passed:
                print(
                    f"拒绝：静止检查未通过：{stationary.reason}（位移 {stationary.translation_span_m * 1000:.2f} mm，"
                    f"转角 {stationary.rotation_span_deg:.2f} deg）。",
                    file=sys.stderr,
                )
                continue
            pose_message = nearest[1]
            if str(pose_message.header.frame_id) != str(frames["base_frame"]):
                print(
                    f"拒绝：右臂位姿父坐标系为 {pose_message.header.frame_id!r}，"
                    f"配置为 {frames['base_frame']!r}。请核实控制器坐标定义，不要仅重命名消息。",
                    file=sys.stderr,
                )
                continue
            camera = camera_snapshot(node.latest_info)
            if output.is_file():
                dataset = load_compatible_dataset(output, config, kind, camera)
            else:
                dataset = new_dataset(config, kind, camera)
            if any(sample.get("image_stamp") == stamp_text(image_message.header.stamp) for sample in dataset["samples"]):
                print("拒绝：这一帧已经保存。", file=sys.stderr)
                continue

            sample_number = len(dataset["samples"]) + 1
            sample: dict[str, Any] = {
                "index": sample_number,
                "image_stamp": stamp_text(image_message.header.stamp),
                "robot_pose_stamp": stamp_text(pose_message.header.stamp),
                "sync_delta_s": sync_delta,
                "base_from_gripper": pose_dict(nearest[2]),
                "camera_from_board": pose_dict(detection.camera_from_board),
                "marker_ids": list(detection.marker_ids),
                "reprojection_rms_px": detection.reprojection_rms_px,
                "stationary_translation_span_m": stationary.translation_span_m,
                "stationary_rotation_span_deg": stationary.rotation_span_deg,
                "stationary_coverage_s": stationary.coverage_s,
                "stationary_sample_count": stationary.sample_count,
                "tool_snapshot": tool_snapshot,
            }
            if bool(capture_config.get("save_images", True)):
                image_directory = output.parent / f"{output.stem}_images"
                image_directory.mkdir(parents=True, exist_ok=True)
                image_path = image_directory / f"sample_{sample_number:03d}.png"
                if not cv2.imwrite(str(image_path), node.latest_bgr):
                    raise CalibrationError(f"failed to save capture image: {image_path}")
                sample["image"] = str(image_path.relative_to(output.parent))
            dataset["samples"].append(sample)
            dataset["updated_at"] = utc_now()
            atomic_write_json(output, dataset)
            print(
                f"已保存 #{sample_number}: markers={len(detection.marker_ids)}, "
                f"reprojection={detection.reprojection_rms_px:.3f}px -> {output}"
            )
    finally:
        cv2.destroyAllWindows()
        if node is not None:
            node.destroy_node()
        rclpy.shutdown()
    count = len(load_json(output)["samples"]) if output.is_file() else 0
    print(f"采集结束：{output}，共 {count} 帧。")
    return 0


def ensure_dataset_matches_result(dataset: Mapping[str, Any], result: Mapping[str, Any]) -> None:
    if dataset.get("board") != result.get("board"):
        raise CalibrationError("validation board geometry differs from the solved result")
    if dataset.get("camera_info") != result.get("camera_info"):
        raise CalibrationError("validation CameraInfo differs from the solved result")
    if dataset.get("frames") != result.get("frames"):
        raise CalibrationError("validation frame contract differs from the solved result")
    if dataset.get("robot_contract") != result.get("robot_contract"):
        raise CalibrationError("validation tool contract differs from the solved result")


def observation_fingerprint(sample: Mapping[str, Any]) -> str:
    payload = {key: sample[key] for key in ("base_from_gripper", "camera_from_board")}
    return hashlib.sha256(json.dumps(payload, sort_keys=True).encode()).hexdigest()


def residual_report(samples, base_from_camera, gripper_from_board, used_indices):
    mounts = [invert_transform(transform_from_dict(sample["base_from_gripper"]))
              @ base_from_camera @ transform_from_dict(sample["camera_from_board"])
              for sample in samples]
    translations, rotations = transform_errors(mounts, gripper_from_board)
    return [{"sample_index": index + 1, "used": index in used_indices,
             "translation_error_mm": translation * 1000.0, "rotation_error_deg": rotation,
             "reprojection_rms_px": float(sample["reprojection_rms_px"]),
             "image": sample.get("image")}
            for index, (sample, translation, rotation) in enumerate(zip(samples, translations, rotations))]


def solve_command(config: Mapping[str, Any]) -> int:
    dataset_file = dataset_path(config, "calibration")
    dataset = load_compatible_dataset(dataset_file, config, "calibration")
    solved = solve_eye_to_hand(dataset["samples"], config["solve"])
    passed, failures = training_quality(solved.metrics, config["quality"])
    transform = pose_dict(solved.base_from_camera)
    document: dict[str, Any] = {
        "schema_version": 1,
        "calibration_type": "eye_to_hand",
        "created_at": utc_now(),
        "parent_frame": str(config["frames"]["base_frame"]),
        "child_frame": str(config["frames"]["camera_frame"]),
        "translation": transform["translation_m"],
        "rotation_xyzw": transform["rotation_xyzw"],
        "matrix_parent_from_child": rounded_matrix(solved.base_from_camera),
        "convention": "p_parent = R_parent_child * p_child + t_parent_child",
        "frames": dict(dataset["frames"]),
        "robot_contract": copy.deepcopy(dataset.get("robot_contract")),
        "board": dict(dataset["board"]),
        "camera_info": copy.deepcopy(dataset["camera_info"]),
        "solver": {
            "method": str(config["solve"]["method"]),
            "source_dataset": portable_path(dataset_file),
            "used_sample_indices": [int(index + 1) for index in solved.used_indices],
            "rejected_sample_indices": [int(index + 1) for index in solved.rejected_indices],
            "training_image_stamps": [sample.get("image_stamp") for sample in dataset["samples"]],
            "training_observation_fingerprints": [observation_fingerprint(sample) for sample in dataset["samples"]],
        },
        "sample_residuals": residual_report(dataset["samples"], solved.base_from_camera,
                                            solved.gripper_from_board, set(solved.used_indices)),
        "estimated_gripper_from_board": pose_dict(solved.gripper_from_board),
        "training_metrics": {key: float(value) for key, value in solved.metrics.items()},
        "validation_metrics": None,
        "quality": {
            "training_passed": passed,
            "validation_passed": False,
            "accepted": False,
            "failures": failures + ["independent validation has not been run"],
        },
    }
    output = result_path(config)
    atomic_write_yaml(output, document)
    print(f"已写入外参候选结果：{output}")
    print(
        f"T_base_camera translation={np.asarray(transform['translation_m'])}, "
        f"quaternion_xyzw={np.asarray(transform['rotation_xyzw'])}"
    )
    print(
        f"训练一致性：{solved.metrics['training_translation_rms_m'] * 1000:.2f} mm RMS, "
        f"{solved.metrics['training_rotation_rms_deg']:.3f} deg RMS；"
        f"运动跨度 {solved.metrics['translation_span_m'] * 1000:.0f} mm / "
        f"{solved.metrics['rotation_span_deg']:.1f} deg。"
    )
    print("逐帧残差（相对于最终估计的腕板安装关系）：")
    for row in document["sample_residuals"]:
        print(f"  #{row['sample_index']:02d} {'used' if row['used'] else 'rejected':8s} "
              f"{row['translation_error_mm']:.2f} mm / {row['rotation_error_deg']:.3f} deg")
    if not passed:
        print("训练质量门未通过：" + "；".join(failures), file=sys.stderr)
        return 1
    print("训练质量门通过。请另采 validation 数据并运行 verify；验证前不会发布 TF。")
    return 0


def verify_command(config: Mapping[str, Any]) -> int:
    output = result_path(config)
    result = load_yaml(output)
    validation_file = dataset_path(config, "validation")
    dataset = load_compatible_dataset(validation_file, config, "validation")
    ensure_dataset_matches_result(dataset, result)
    solver = result.get("solver", {})
    fingerprints = set(solver.get("training_observation_fingerprints", []))
    if not fingerprints:
        raise CalibrationError("result lacks training observation fingerprints; rerun solve before independent validation")
    stamps = set(solver.get("training_image_stamps", [])) - {None}
    if any(observation_fingerprint(sample) in fingerprints
           or (sample.get("image_stamp") is not None and sample["image_stamp"] in stamps)
           for sample in dataset["samples"]):
        raise CalibrationError("validation reuses training observations; collect genuinely independent poses")
    base_from_camera = transform_from_dict(result)
    gripper_from_board = transform_from_dict(result["estimated_gripper_from_board"])
    metrics = validation_metrics(dataset["samples"], base_from_camera, gripper_from_board)
    passed, failures = validation_quality(metrics, config["quality"])
    training_passed, training_failures = training_quality(result["training_metrics"], config["quality"])
    all_failures = training_failures + failures
    result["verified_at"] = utc_now()
    result["validation_dataset"] = portable_path(validation_file)
    result["validation_metrics"] = metrics
    result["validation_sample_residuals"] = residual_report(
        dataset["samples"], base_from_camera, gripper_from_board, set(range(len(dataset["samples"]))))
    result["quality"] = {
        "training_passed": training_passed,
        "validation_passed": passed,
        "accepted": training_passed and passed,
        "failures": all_failures,
    }
    atomic_write_yaml(output, result)
    print(
        f"独立验证：{metrics['validation_translation_rms_m'] * 1000:.2f} mm RMS "
        f"(max {metrics['validation_translation_max_m'] * 1000:.2f} mm), "
        f"{metrics['validation_rotation_rms_deg']:.3f} deg RMS "
        f"(max {metrics['validation_rotation_max_deg']:.3f} deg)"
    )
    if not result["quality"]["accepted"]:
        print("外参未通过发布门：" + "；".join(all_failures), file=sys.stderr)
        return 1
    print(f"外参已通过训练和独立验证，可发布：{output}")
    return 0


def publish_command(config: Mapping[str, Any], dry_run: bool) -> int:
    if "tf_publish" not in config:
        raise CalibrationError("missing tf_publish configuration; legacy configuration is offline diagnosis-only")
    result = load_yaml(result_path(config))
    if not bool(result.get("quality", {}).get("accepted", False)):
        raise CalibrationError("result has not passed both training and independent validation")
    expected_frames = {
        "base_frame": str(config["frames"]["base_frame"]),
        "camera_frame": str(config["frames"]["camera_frame"]),
        "robot_pose_child_frame": str(config["frames"]["robot_pose_child_frame"]),
    }
    if result.get("frames") != expected_frames or result.get("board") != board_snapshot(config["board"]):
        raise CalibrationError("accepted result no longer matches the current frame/board configuration")
    if (result.get("parent_frame") != expected_frames["base_frame"]
            or result.get("child_frame") != expected_frames["camera_frame"]):
        raise CalibrationError("result parent/child fields disagree with its frame contract")
    if result.get("robot_contract") != robot_contract(config):
        raise CalibrationError("result does not match the current tool contract")
    training_passed, _ = training_quality(result["training_metrics"], config["quality"])
    validation_passed, _ = validation_quality(result["validation_metrics"], config["quality"])
    if not training_passed or not validation_passed:
        raise CalibrationError("result fails the current quality thresholds")
    from .tf_publish import lookup_camera_internal_transform
    root_from_optical = lookup_camera_internal_transform(config)
    publish_pose = pose_dict(base_from_camera_root(transform_from_dict(result), root_from_optical))
    translation = publish_pose["translation_m"]
    quaternion = publish_pose["rotation_xyzw"]
    arguments = [
        "ros2",
        "run",
        "tf2_ros",
        "static_transform_publisher",
        "--x",
        str(translation[0]),
        "--y",
        str(translation[1]),
        "--z",
        str(translation[2]),
        "--qx",
        str(quaternion[0]),
        "--qy",
        str(quaternion[1]),
        "--qz",
        str(quaternion[2]),
        "--qw",
        str(quaternion[3]),
        "--frame-id",
        str(result["parent_frame"]),
        "--child-frame-id",
        str(config["tf_publish"]["camera_root_frame"]),
    ]
    print("保留相机内部 TF；发布的是 base <- camera_root，非原始 optical 外参。")
    print(" ".join(shlex.quote(item) for item in arguments), flush=True)
    if dry_run:
        return 0
    os.execvp(arguments[0], arguments)
    return 0  # pragma: no cover


def doctor_command(config: Mapping[str, Any]) -> int:
    board = config["board"]
    print("外参模式：固定相机 eye-to-hand")
    print(
        f"变换：{config['frames']['base_frame']} <- {config['frames']['camera_frame']}；"
        f"右臂末端：{config['frames']['robot_pose_child_frame']}"
    )
    print(
        f"标定板：{board['dictionary']}，{board['markers_x']}x{board['markers_y']}，"
        f"ID {board['first_marker_id']}.."
        f"{board['first_marker_id'] + board['markers_x'] * board['markers_y'] - 1}，"
        f"黑块 {float(board['marker_length_m']) * 1000:.2f} mm，"
        f"间距 {float(board['marker_separation_m']) * 1000:.2f} mm"
    )
    print(f"OpenCV {cv2.__version__}，aruco=OK，calibrateHandEye=OK")
    missing: list[str] = []
    for module_name in ("numpy", "yaml", "PIL", "rclpy", "cv_bridge", "sensor_msgs", "geometry_msgs",
                        "tf2_ros", "tf2_msgs", "rosidl_runtime_py"):
        try:
            importlib.import_module(module_name)
        except ImportError:
            missing.append(module_name)
    if shutil.which("ros2") is None:
        missing.append("ros2 executable")
    if missing:
        raise CalibrationError("缺少运行依赖或 ROS 环境未加载：" + ", ".join(missing))
    print("Python/ROS 运行依赖：OK")
    if "robot" in config:
        from .tool_monitor import tool_service_class
        tool_service_class(config["robot"])
        print(f"工具查询类型：OK；采集时只读检查 {config['robot']['tool_service']}")
    print("注意：doctor 只能检查软件配置；请用尺实测标定板并在 collect 时确认。")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description="固定 Gemini 2 + 右腕 ArUco 外参标定")
    parser.add_argument("--config", default=str(DEFAULT_CONFIG), help="外参标定配置 YAML")
    subparsers = parser.add_subparsers(dest="command", required=True)
    subparsers.add_parser("doctor", help="检查配置和 OpenCV 能力")

    generate = subparsers.add_parser("generate-board", help="生成与配置相同的 A4 参考板 PNG")
    generate.add_argument(
        "--output",
        default="artifacts/aruco_board_a4.png",
        help="输出 PNG（相对仓库根目录）",
    )
    generate.add_argument("--dpi", type=int, default=600)

    collect_parser = subparsers.add_parser("collect", help="交互采集标定或验证姿态")
    collect_parser.add_argument("--set", choices=("calibration", "validation"), default="calibration")
    collect_parser.add_argument(
        "--board-measured",
        action="store_true",
        help="确认已实测黑块边长和间距，并已写入配置",
    )
    subparsers.add_parser("solve", help="求解 T_base_camera 并执行训练质量检查")
    subparsers.add_parser("verify", help="用独立数据验证，并决定是否允许发布")
    publish = subparsers.add_parser("publish-tf", help="发布通过验证的静态 TF")
    publish.add_argument("--dry-run", action="store_true", help="只读查询相机内部 TF 并打印命令，不发布；需启动相机")
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    parser = build_parser()
    arguments = parser.parse_args(argv)
    try:
        config = load_config(repository_path(arguments.config))
        if arguments.command == "doctor":
            return doctor_command(config)
        if arguments.command == "generate-board":
            if arguments.dpi < 150:
                raise CalibrationError("board DPI must be at least 150")
            output = repository_path(arguments.output)
            generate_board_png(config["board"], output, arguments.dpi)
            print(f"已生成：{output}。打印时必须 100%/Actual size，打印后再次实测黑块边长。")
            return 0
        if arguments.command == "collect":
            if not arguments.board_measured:
                raise CalibrationError(
                    "尚未确认标定板实测尺寸；更新 YAML 后请添加 --board-measured"
                )
            return collect(config, arguments.set)
        if arguments.command == "solve":
            return solve_command(config)
        if arguments.command == "verify":
            return verify_command(config)
        if arguments.command == "publish-tf":
            return publish_command(config, arguments.dry_run)
        parser.error(f"unknown command: {arguments.command}")
    except (CalibrationError, KeyError, ValueError, cv2.error) as error:
        print(f"外参标定失败：{error}", file=sys.stderr)
        return 2
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
