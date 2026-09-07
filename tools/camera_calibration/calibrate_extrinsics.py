#!/usr/bin/env python3
"""Calibrate a torso-mounted camera using a printed R8 docking fixture."""

from __future__ import annotations

import argparse
from datetime import datetime
import hashlib
from pathlib import Path
import sys
import time

ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(ROOT))

import cv2
import numpy as np
import yaml

from linkerbot.calibration_board import generate_board, validate_board
from linkerbot.extrinsic_calibration import (
    StableObservations, camera_model, detect_board, estimate_pose, invert_transform,
    quaternion_xyzw, result_document, saved_transform, validate_config,
)
from linkerbot.robot_kinematics import RobotChain
from linkerbot.imu_sampling import ImuWindow
from linkerbot.runtime import ConfigurationError, load_yaml, repository_path


def imu_window(config: dict) -> tuple[ImuWindow, str]:
    settings = dict(config.get("imu", {}))
    camera_path = settings.pop("camera_config", None)
    if not isinstance(camera_path, str):
        raise ConfigurationError("imu.camera_config must name the camera configuration")
    camera = load_yaml(camera_path)
    parameters, descriptor = camera.get("parameters", {}), camera.get("imu", {})
    if (parameters.get("enable_sync_output_accel_gyro") is not True
            or parameters.get("time_domain") != "global"
            or parameters.get("timestamp_clock_type") != "realtime"):
        raise ConfigurationError("IMU calibration requires synchronized output with global/realtime timestamps")
    topic = descriptor.get("topic")
    if not isinstance(topic, str) or not topic or descriptor.get("message_type") != "sensor_msgs/msg/Imu":
        raise ConfigurationError("Camera configuration must define an Imu topic and message_type")
    settings["frame_id"] = descriptor.get("frame_id")
    return ImuWindow(settings), topic


def offline_imu(config: dict, path: str, start_ns: int, end_ns: int) -> dict:
    window, topic = imu_window(config)
    document = load_yaml(path)
    if document.get("schema_version") != 1 or not isinstance(document.get("samples"), list):
        raise ConfigurationError("IMU file requires schema_version: 1 and a samples list in SI units")
    for sample in document["samples"]:
        if not isinstance(sample, dict) or not window.add(
            sample.get("stamp_ns"), sample.get("frame_id"),
            sample.get("linear_acceleration_m_s2"), sample.get("angular_velocity_rad_s")
        ):
            raise ConfigurationError(f"Invalid IMU file sample: {window.last_add_reason}")
        if window.last_add_reason == "backward_stamp_reset":
            raise ConfigurationError("IMU file timestamps must be strictly increasing")
    report = window.export(start_ns, end_ns)
    report["topic"] = topic
    return report


def verify_mesh(config: dict) -> None:
    fixture = config["fixture"]
    path = repository_path(fixture["source_mesh_path"])
    actual = hashlib.sha256(path.read_bytes()).hexdigest()
    if actual != fixture["source_mesh_sha256"]:
        raise ConfigurationError("R8 mesh differs from the measured fixture; regenerate its geometry first")


def verify_print(config: dict, path: Path) -> dict:
    document = load_yaml(path)
    if document.get("board") != validate_board(config["board"]):
        raise ConfigurationError("Printed board geometry differs from configuration; regenerate AND reprint")
    printed_fixture = document.get("fixture", {})
    if any(printed_fixture.get(k) != v for k, v in config["fixture"].items()):
        raise ConfigurationError("Printed R8 fixture differs from configuration; regenerate AND reprint")
    return document


def robot_pose(config: dict, path: str) -> tuple[np.ndarray, dict]:
    joints = load_yaml(path)
    chain = RobotChain(repository_path(config["robot"]["urdf"]), config["frames"]["base"],
                       config["frames"]["flange"])
    pose = chain.forward(joints)
    return pose, {"source": "offline_joint_snapshot", "joint_positions_rad": joints["joint_positions_rad"],
                  "urdf_sha256": chain.urdf_sha256, "base_frame": config["frames"]["base"],
                  "tip_frame": config["frames"]["flange"],
                  "time_alignment": "operator must capture joints and image in the same stationary pose"}


def annotate(image: np.ndarray, pixels: np.ndarray, ids: list[int], pose: dict,
             matrix: np.ndarray, distortion: np.ndarray) -> np.ndarray:
    debug = image.copy()
    corners = [quad.reshape(1, 4, 2).astype(np.float32) for quad in pixels.reshape(-1, 4, 2)]
    cv2.aruco.drawDetectedMarkers(debug, corners, np.asarray(ids, dtype=np.int32))
    cv2.drawFrameAxes(debug, matrix, distortion, pose["rvec"], pose["tvec"], 0.04)
    return debug


def save_result(config: dict, pose: dict, statistics: dict, intrinsics: dict,
                base_flange: np.ndarray, robot: dict, image: np.ndarray, debug: np.ndarray,
                board_geometry: dict, output: str | None, imu_report: dict | None = None) -> Path:
    destination = repository_path(output) if output else (
        repository_path(config["output_directory"]) / datetime.now().strftime("%Y%m%d_%H%M%S_%f"))
    destination.mkdir(parents=True, exist_ok=False)
    result = result_document(config, pose, statistics, intrinsics, base_flange, robot)
    result["created_at"] = datetime.now().astimezone().isoformat()
    result["printed_board_geometry"] = board_geometry
    result["imu"] = {"used_for_pose_estimation": False, "stationarity_checked": imu_report is not None}
    if imu_report is not None:
        result["imu"]["assessment"] = imu_report["assessment"]
        (destination / "imu_samples.yaml").write_text(yaml.safe_dump(imu_report, sort_keys=False), encoding="utf-8")
    for name, data in (("extrinsics.yaml", result), ("camera_info.yaml", intrinsics)):
        (destination / name).write_text(yaml.safe_dump(data, sort_keys=False), encoding="utf-8")
    if not cv2.imwrite(str(destination / "image.png"), image) or not cv2.imwrite(str(destination / "overlay.png"), debug):
        raise OSError("Unable to save calibration images")
    print(f"Saved: {destination / 'extrinsics.yaml'}")
    print("base <- color optical translation (m):", result["transform"]["translation_m"])
    print("quaternion (x y z w):", result["transform"]["quaternion_xyzw"])
    print(f"Reprojection RMS: {pose['rms_px']:.4f} px; samples: {statistics['sample_count']}")
    print("Geometric fit is not an independent check of joint zero offsets or mechanical alignment.")
    return destination


def solve_image(arguments, config: dict, board_geometry: dict) -> None:
    base_flange, robot = robot_pose(config, arguments.joints)
    image = cv2.imread(str(repository_path(arguments.image)))
    if image is None:
        raise ConfigurationError(f"Cannot read image: {arguments.image}")
    intrinsics = load_yaml(arguments.camera_info)
    matrix, distortion = camera_model(intrinsics, image.shape)
    ids, objects, pixels = detect_board(image, config["board"])
    pose = estimate_pose(objects, pixels, matrix, distortion, config["quality"])
    imu_report = None
    if arguments.imu_file:
        if (arguments.image_stamp_ns is None
                and (arguments.imu_start_stamp_ns is None or arguments.imu_end_stamp_ns is None)):
            raise ConfigurationError("--imu-file requires --image-stamp-ns or both explicit IMU interval stamps")
        window_ns = int(round(config["imu"]["window_s"] * 1_000_000_000))
        imu_start = (arguments.imu_start_stamp_ns if arguments.imu_start_stamp_ns is not None
                     else arguments.image_stamp_ns - window_ns)
        imu_end = (arguments.imu_end_stamp_ns if arguments.imu_end_stamp_ns is not None
                   else arguments.image_stamp_ns)
        if imu_start <= 0 or imu_end < imu_start:
            raise ConfigurationError("IMU interval must be positive and ordered")
        imu_report = offline_imu(config, arguments.imu_file, imu_start, imu_end)
        if not imu_report["assessment"]["ready"]:
            raise ConfigurationError(f"IMU window rejected: {imu_report['assessment']['reason']}")
    statistics = {"sample_count": 1, "marker_ids": ids, "reprojection_rms_px": pose["rms_px"],
                  "temporal_stability_verified": False, "mode": "offline_single_image"}
    save_result(config, pose, statistics, intrinsics, base_flange, robot, image,
                annotate(image, pixels, ids, pose, matrix, distortion), board_geometry, arguments.output, imu_report)


def capture(arguments, config: dict, board_geometry: dict) -> None:
    import rclpy
    from rclpy.node import Node
    from rclpy.qos import qos_profile_sensor_data
    from sensor_msgs.msg import CameraInfo, Image, Imu
    from cv_bridge import CvBridge

    base_flange, robot = robot_pose(config, arguments.joints)
    rclpy.init()
    node = Node("r8_camera_extrinsic_calibrator")
    bridge = CvBridge()
    state = {"image": None, "info": None, "receipt": 0.0, "imu_receipt": 0.0}
    inertial, imu_topic = imu_window(config) if arguments.with_imu else (None, None)
    window = StableObservations(config["quality"])

    def on_imu(message):
        stamp = message.header.stamp.sec * 1_000_000_000 + message.header.stamp.nanosec
        a, w = message.linear_acceleration, message.angular_velocity
        age = (node.get_clock().now().nanoseconds - stamp) / 1e9
        if (age < -0.1 or age > config["quality"]["max_image_age_s"]
                or not inertial.add(stamp, message.header.frame_id, [a.x, a.y, a.z], [w.x, w.y, w.z])):
            # Rejected input must not leave an old accepted window eligible for saving.
            if inertial.last_add_reason != "duplicate" or age < -0.1 or age > config["quality"]["max_image_age_s"]:
                inertial.clear()
                window.clear()
            return
        if inertial.last_add_reason == "backward_stamp_reset":
            window.clear()
        state["imu_receipt"] = time.monotonic()

    def on_image(message):
        state["image"], state["receipt"] = message, time.monotonic()

    node.create_subscription(Image, config["topics"]["color"], on_image, qos_profile_sensor_data)
    node.create_subscription(CameraInfo, config["topics"]["camera_info"],
                             lambda message: state.update(info=message), qos_profile_sensor_data)
    if inertial is not None:
        node.create_subscription(Imu, imu_topic, on_imu, qos_profile_sensor_data)
    last_stamp, last_report, last_intrinsics = None, "", None
    deadline = time.monotonic() + config["quality"]["capture_timeout_s"]
    print("Keep the R8 face flush, robot joints stationary, and camera/board visible. S: save; Esc: exit.")
    print("Joint snapshot is fixed for this session; restart with new angles after moving the arm.")
    try:
        while rclpy.ok() and time.monotonic() < deadline:
            rclpy.spin_once(node, timeout_sec=0.05)
            message, info = state["image"], state["info"]
            if message is None or info is None:
                continue
            stamp = message.header.stamp.sec * 1_000_000_000 + message.header.stamp.nanosec
            age = (node.get_clock().now().nanoseconds - stamp) / 1e9
            if (age < -0.1 or age > config["quality"]["max_image_age_s"]
                    or time.monotonic() - state["receipt"] > config["quality"]["max_image_age_s"]):
                window.clear()
                continue
            if stamp == last_stamp:
                if not arguments.headless and cv2.waitKey(1) & 0xFF == 27:
                    return
                continue
            last_stamp = stamp
            if message.header.frame_id != config["frames"]["optical"] or info.header.frame_id != message.header.frame_id:
                raise ConfigurationError("Color image/CameraInfo frame does not match configured optical frame")
            intrinsics = {"image_width": info.width, "image_height": info.height,
                          "distortion_model": info.distortion_model,
                          "camera_matrix": {"rows": 3, "cols": 3, "data": list(info.k)},
                          "distortion_coefficients": {"rows": 1, "cols": len(info.d), "data": list(info.d)}}
            if last_intrinsics != intrinsics:
                window.clear()
            last_intrinsics = intrinsics
            image = bridge.imgmsg_to_cv2(message, desired_encoding="bgr8")
            matrix, distortion = camera_model(intrinsics, image.shape)
            debug, finished, statistics, imu_report = image.copy(), None, None, None
            try:
                ids, objects, pixels = detect_board(image, config["board"])
                pose = estimate_pose(objects, pixels, matrix, distortion, config["quality"])
            except ValueError as error:
                window.clear()
                status = str(error)
            else:
                window.add(stamp, ids, pixels, pose)
                debug = annotate(image, pixels, ids, pose, matrix, distortion)
                try:
                    candidate, statistics = window.finish(objects, matrix, distortion)
                    if inertial is not None:
                        imu_report = inertial.export(statistics["first_image_stamp_ns"], statistics["last_image_stamp_ns"])
                        imu_report["topic"] = imu_topic
                        if time.monotonic() - state["imu_receipt"] > config["quality"]["max_image_age_s"]:
                            window.clear()
                            raise ValueError("IMU missing or stale")
                        if not imu_report["assessment"]["ready"]:
                            reason = imu_report["assessment"]["reason"]
                            window.clear()
                            raise ValueError(f"IMU rejected: {reason}")
                    finished = candidate
                    statistics.update(temporal_stability_verified=True, mode="live_images_offline_joints")
                    status = f"Ready: {finished['rms_px']:.3f} px"
                except ValueError as error:
                    status = str(error)
            if status != last_report:
                print(status, flush=True)
                last_report = status
            key = -1
            if not arguments.headless:
                cv2.putText(debug, status, (8, 25), cv2.FONT_HERSHEY_SIMPLEX, .5, (0, 160, 0), 1)
                cv2.imshow("R8 camera calibration", debug)
                key = cv2.waitKey(1) & 0xFF
                if key == 27:
                    return
            if finished is not None and (arguments.headless or key in (ord("s"), ord("S"))):
                save_result(config, finished, statistics, intrinsics, base_flange, robot, image, debug,
                            board_geometry, arguments.output, imu_report)
                return
        raise ConfigurationError("Capture timed out; check image stream, marker visibility, and stability")
    finally:
        if not arguments.headless:
            cv2.destroyAllWindows()
        node.destroy_node()
        rclpy.shutdown()


def publish_result(path: str) -> None:
    import rclpy
    from rclpy.node import Node
    from rclpy.time import Time
    from geometry_msgs.msg import TransformStamped
    from tf2_ros import Buffer, TransformListener, StaticTransformBroadcaster, TransformException

    document = load_yaml(path)
    base_camera = saved_transform(document)
    parent = document["transform"]["parent_frame"]
    optical = document["transform"]["child_frame"]
    camera_root = document["camera_root_frame"]
    rclpy.init()
    node = Node("r8_camera_extrinsic_publisher")
    buffer = Buffer()
    listener = TransformListener(buffer, node)
    broadcaster = StaticTransformBroadcaster(node)
    try:
        deadline = time.monotonic() + 10.0
        root_optical = None
        while rclpy.ok() and time.monotonic() < deadline:
            rclpy.spin_once(node, timeout_sec=.1)
            try:
                root_optical = buffer.lookup_transform(camera_root, optical, Time())
                break
            except TransformException:
                continue
        if root_optical is None:
            raise ConfigurationError("Start the Orbbec driver first: camera root <- optical TF is missing")
        # Attach the root of the driver's camera tree, avoiding a second parent for the optical frame.
        if buffer.can_transform(parent, camera_root, Time()):
            raise ConfigurationError("A base-to-camera TF already exists; stop its publisher before replacing it")
        q = root_optical.transform.rotation
        quaternion = np.array([q.x, q.y, q.z, q.w], dtype=float)
        quaternion /= np.linalg.norm(quaternion)
        vector = quaternion[:3]
        skew = np.array([[0, -vector[2], vector[1]], [vector[2], 0, -vector[0]],
                         [-vector[1], vector[0], 0]])
        root_camera = np.eye(4)
        root_camera[:3, :3] = np.eye(3) + 2 * quaternion[3] * skew + 2 * skew @ skew
        t = root_optical.transform.translation
        root_camera[:3, 3] = [t.x, t.y, t.z]
        base_root = base_camera @ invert_transform(root_camera)
        message = TransformStamped()
        message.header.frame_id, message.child_frame_id = parent, camera_root
        message.header.stamp = node.get_clock().now().to_msg()
        t = message.transform.translation
        t.x, t.y, t.z = map(float, base_root[:3, 3])
        q = message.transform.rotation
        q.x, q.y, q.z, q.w = quaternion_xyzw(base_root[:3, :3])
        broadcaster.sendTransform(message)
        print(f"Publishing fixed TF {parent} -> {camera_root}; Ctrl+C to stop", flush=True)
        rclpy.spin(node)
    finally:
        node.destroy_node()
        rclpy.shutdown()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", default="config/calibration/extrinsics.yaml")
    commands = parser.add_subparsers(dest="command", required=True)
    generate = commands.add_parser("generate", help="Generate an actual-size A4 PDF and fixture geometry")
    generate.add_argument("--output")
    for name in ("solve", "capture"):
        command = commands.add_parser(name, help="Solve from an image" if name == "solve" else "Collect live color images")
        command.add_argument("--joints", required=True, help="Same-pose measured right-arm joint YAML")
        command.add_argument("--board-geometry", help="Geometry YAML shipped with the printed PDF")
        command.add_argument("--output", help="New result directory (must not already exist)")
        if name == "solve":
            command.add_argument("--image", required=True)
            command.add_argument("--camera-info", required=True)
            command.add_argument("--imu-file", help="Optional synchronized IMU window YAML, expressed in SI units")
            command.add_argument("--image-stamp-ns", type=int, help="Original RGB timestamp; checks the preceding configured IMU window")
            command.add_argument("--imu-start-stamp-ns", type=int, help="Explicit IMU interval start in the RGB/IMU clock")
            command.add_argument("--imu-end-stamp-ns", type=int, help="Explicit IMU interval end in the RGB/IMU clock")
        else:
            command.add_argument("--headless", action="store_true", help="Save automatically when stable")
            command.add_argument("--with-imu", action="store_true", help="Require stationary IMU coverage and save raw samples")
    check_imu = commands.add_parser("check-imu", help="Assess recorded IMU without a robot or image")
    check_imu.add_argument("--imu-file", required=True)
    check_imu.add_argument("--start-stamp-ns", required=True, type=int)
    check_imu.add_argument("--end-stamp-ns", required=True, type=int)
    publisher = commands.add_parser("publish", help="Publish saved camera-root TF; never sends robot motion")
    publisher.add_argument("--result", required=True)
    arguments = parser.parse_args()
    try:
        if arguments.command == "publish":
            publish_result(arguments.result)
            return 0
        config = load_yaml(arguments.config)
        if arguments.command == "solve" and ((arguments.imu_start_stamp_ns is None) != (arguments.imu_end_stamp_ns is None)):
            raise ConfigurationError("--imu-start-stamp-ns and --imu-end-stamp-ns must be supplied together")
        if arguments.command == "check-imu":
            report = offline_imu(config, arguments.imu_file, arguments.start_stamp_ns, arguments.end_stamp_ns)
            print(yaml.safe_dump(report["assessment"], sort_keys=False))
            return 0 if report["assessment"]["ready"] else 2
        validate_config(config)
        verify_mesh(config)
        if arguments.command == "generate":
            output = repository_path(arguments.output) if arguments.output else repository_path(config["output_directory"]) / "board"
            for kind, path in generate_board(config["board"], output, fixture=config["fixture"]).items():
                print(f"{kind}: {path}")
            return 0
        geometry_path = repository_path(arguments.board_geometry) if arguments.board_geometry else repository_path(config["output_directory"]) / "board/board_geometry.yaml"
        geometry = verify_print(config, geometry_path)
        if arguments.command == "solve":
            solve_image(arguments, config, geometry)
        else:
            capture(arguments, config, geometry)
        return 0
    except KeyboardInterrupt:
        return 130
    except (ConfigurationError, ValueError, OSError, ImportError, cv2.error) as error:
        print(f"External calibration failed: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
