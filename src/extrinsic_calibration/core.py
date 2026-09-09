"""Pure geometry, ArUco detection, persistence, and eye-to-hand solving.

The robot/camera adapters live in ``cli.py``.  Keeping this module ROS-free
makes the transform convention and solver testable with synthetic data.
"""

from __future__ import annotations

import json
import math
import os
from dataclasses import dataclass
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Mapping, Sequence

import cv2
import numpy as np
import yaml


SCHEMA_VERSION = 1
SUPPORTED_DISTORTION_MODELS = {"plumb_bob", "rational_polynomial", "equidistant"}


class CalibrationError(RuntimeError):
    """Raised when calibration input is incomplete, unsafe, or inconsistent."""


@dataclass(frozen=True)
class BoardDetection:
    camera_from_board: np.ndarray
    marker_ids: tuple[int, ...]
    corners: tuple[np.ndarray, ...]
    rejected: tuple[np.ndarray, ...]
    reprojection_rms_px: float
    rvec: np.ndarray
    tvec: np.ndarray


@dataclass(frozen=True)
class SolveResult:
    base_from_camera: np.ndarray
    gripper_from_board: np.ndarray
    used_indices: tuple[int, ...]
    rejected_indices: tuple[int, ...]
    translation_errors_m: tuple[float, ...]
    rotation_errors_deg: tuple[float, ...]
    metrics: Mapping[str, float]


def utc_now() -> str:
    return datetime.now(timezone.utc).isoformat(timespec="seconds")


def require_mapping(value: Any, name: str) -> Mapping[str, Any]:
    if not isinstance(value, Mapping):
        raise CalibrationError(f"'{name}' must be a YAML mapping")
    return value


def require_positive(value: Any, name: str) -> float:
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise CalibrationError(f"'{name}' must be a positive number")
    result = float(value)
    if not math.isfinite(result) or result <= 0.0:
        raise CalibrationError(f"'{name}' must be finite and positive")
    return result


def load_config(path: Path) -> dict[str, Any]:
    if not path.is_file():
        raise CalibrationError(f"configuration file does not exist: {path}")
    with path.open(encoding="utf-8") as stream:
        document = yaml.safe_load(stream)
    config = dict(require_mapping(document, "configuration root"))
    if config.get("schema_version") != SCHEMA_VERSION:
        raise CalibrationError(f"unsupported configuration schema: {config.get('schema_version')!r}")
    if config.get("calibration_type") != "eye_to_hand":
        raise CalibrationError("only fixed-camera eye_to_hand calibration is supported")

    frames = require_mapping(config.get("frames"), "frames")
    topics = require_mapping(config.get("topics"), "topics")
    board = require_mapping(config.get("board"), "board")
    capture = require_mapping(config.get("capture"), "capture")
    solve = require_mapping(config.get("solve"), "solve")
    quality = require_mapping(config.get("quality"), "quality")
    for name in ("base_frame", "camera_frame", "robot_pose_child_frame"):
        if not isinstance(frames.get(name), str) or not frames[name].strip():
            raise CalibrationError(f"'frames.{name}' must be a non-empty string")
    for name in ("color_image", "color_camera_info", "robot_pose"):
        if not isinstance(topics.get(name), str) or not str(topics[name]).startswith("/"):
            raise CalibrationError(f"'topics.{name}' must be an absolute ROS topic")

    if not hasattr(cv2, "aruco") or not hasattr(cv2, "calibrateHandEye"):
        raise CalibrationError(
            "OpenCV ArUco/hand-eye modules are unavailable; install the matching opencv-contrib package"
        )
    dictionary_name = board.get("dictionary")
    if not isinstance(dictionary_name, str) or not hasattr(cv2.aruco, dictionary_name):
        raise CalibrationError(f"unsupported ArUco dictionary: {dictionary_name!r}")
    for name in ("markers_x", "markers_y"):
        value = board.get(name)
        if isinstance(value, bool) or not isinstance(value, int) or value < 1:
            raise CalibrationError(f"'board.{name}' must be a positive integer")
    require_positive(board.get("marker_length_m"), "board.marker_length_m")
    require_positive(board.get("marker_separation_m"), "board.marker_separation_m")
    first_id = board.get("first_marker_id")
    if isinstance(first_id, bool) or not isinstance(first_id, int) or first_id < 0:
        raise CalibrationError("'board.first_marker_id' must be a non-negative integer")

    total_markers = int(board["markers_x"]) * int(board["markers_y"])
    minimum_markers = capture.get("minimum_markers")
    if not isinstance(minimum_markers, int) or not 1 <= minimum_markers <= total_markers:
        raise CalibrationError("'capture.minimum_markers' must fit the configured board")
    for name in (
        "maximum_reprojection_rms_px",
        "maximum_sync_delta_s",
        "stationary_window_s",
        "maximum_stationary_translation_m",
        "maximum_stationary_rotation_deg",
    ):
        require_positive(capture.get(name), f"capture.{name}")
    for name in ("calibration_dataset", "validation_dataset"):
        if not isinstance(capture.get(name), str) or not capture[name].strip():
            raise CalibrationError(f"'capture.{name}' must be a path string")

    methods = {"TSAI", "PARK", "HORAUD", "ANDREFF", "DANIILIDIS"}
    if solve.get("method") not in methods:
        raise CalibrationError(f"'solve.method' must be one of {sorted(methods)}")
    if not isinstance(solve.get("minimum_samples"), int) or solve["minimum_samples"] < 3:
        raise CalibrationError("'solve.minimum_samples' must be an integer >= 3")
    if not isinstance(solve.get("maximum_outlier_iterations"), int) or solve["maximum_outlier_iterations"] < 0:
        raise CalibrationError("'solve.maximum_outlier_iterations' must be a non-negative integer")
    for name in ("outlier_translation_m", "outlier_rotation_deg"):
        require_positive(solve.get(name), f"solve.{name}")
    if not isinstance(solve.get("result"), str) or not solve["result"].strip():
        raise CalibrationError("'solve.result' must be a path string")
    for name, value in quality.items():
        require_positive(value, f"quality.{name}")
    return config


def make_board(board_config: Mapping[str, Any]):
    dictionary = cv2.aruco.getPredefinedDictionary(
        int(getattr(cv2.aruco, str(board_config["dictionary"])))
    )
    board = cv2.aruco.GridBoard_create(
        int(board_config["markers_x"]),
        int(board_config["markers_y"]),
        float(board_config["marker_length_m"]),
        float(board_config["marker_separation_m"]),
        dictionary,
        int(board_config["first_marker_id"]),
    )
    return dictionary, board


def aruco_detector_parameters():
    parameters = cv2.aruco.DetectorParameters_create()
    parameters.cornerRefinementMethod = cv2.aruco.CORNER_REFINE_SUBPIX
    parameters.cornerRefinementWinSize = 5
    parameters.cornerRefinementMaxIterations = 50
    parameters.cornerRefinementMinAccuracy = 0.01
    return parameters


def _matched_board_points(board, corners, ids) -> tuple[np.ndarray, np.ndarray, tuple[int, ...], tuple[np.ndarray, ...]]:
    if ids is None:
        return np.empty((0, 3)), np.empty((0, 2)), (), ()
    board_lookup = {
        int(marker_id): np.asarray(points, dtype=np.float64).reshape(4, 3)
        for marker_id, points in zip(np.asarray(board.ids).reshape(-1), board.objPoints)
    }
    object_points: list[np.ndarray] = []
    image_points: list[np.ndarray] = []
    kept_ids: list[int] = []
    kept_corners: list[np.ndarray] = []
    for marker_id, marker_corners in zip(np.asarray(ids).reshape(-1), corners):
        marker_id = int(marker_id)
        if marker_id not in board_lookup:
            continue
        pixel_corners = np.asarray(marker_corners, dtype=np.float64).reshape(4, 2)
        object_points.append(board_lookup[marker_id])
        image_points.append(pixel_corners)
        kept_ids.append(marker_id)
        kept_corners.append(np.asarray(marker_corners))
    if not object_points:
        return np.empty((0, 3)), np.empty((0, 2)), (), ()
    return (
        np.concatenate(object_points, axis=0),
        np.concatenate(image_points, axis=0),
        tuple(kept_ids),
        tuple(kept_corners),
    )


def detect_board_pose(
    image_bgr: np.ndarray,
    camera_matrix: np.ndarray,
    distortion: np.ndarray,
    distortion_model: str,
    board_config: Mapping[str, Any],
    minimum_markers: int = 1,
) -> BoardDetection | None:
    """Detect the configured board and return ``camera <- board``."""
    if distortion_model not in SUPPORTED_DISTORTION_MODELS:
        raise CalibrationError(f"unsupported CameraInfo distortion model: {distortion_model!r}")
    camera_matrix = np.asarray(camera_matrix, dtype=np.float64).reshape(3, 3)
    distortion = np.asarray(distortion, dtype=np.float64).reshape(-1, 1)
    dictionary, board = make_board(board_config)
    gray = cv2.cvtColor(image_bgr, cv2.COLOR_BGR2GRAY)
    corners, ids, rejected = cv2.aruco.detectMarkers(
        gray, dictionary, parameters=aruco_detector_parameters()
    )
    object_points, image_points, kept_ids, kept_corners = _matched_board_points(
        board, corners, ids
    )
    if len(kept_ids) < minimum_markers:
        return None
    if distortion_model == "equidistant":
        if distortion.size != 4:
            raise CalibrationError("equidistant CameraInfo must contain four D coefficients")
        solve_points = cv2.fisheye.undistortPoints(
            image_points.reshape(-1, 1, 2), camera_matrix, distortion
        ).reshape(-1, 2)
        success, rvec, tvec = cv2.solvePnP(
            object_points, solve_points, np.eye(3), None, flags=cv2.SOLVEPNP_ITERATIVE
        )
        projected, _ = cv2.fisheye.projectPoints(
            object_points.reshape(1, -1, 3), rvec, tvec, camera_matrix, distortion
        )
    else:
        success, rvec, tvec = cv2.solvePnP(
            object_points,
            image_points,
            camera_matrix,
            distortion,
            flags=cv2.SOLVEPNP_ITERATIVE,
        )
        projected, _ = cv2.projectPoints(
            object_points, rvec, tvec, camera_matrix, distortion
        )
    if not success or not np.all(np.isfinite(rvec)) or not np.all(np.isfinite(tvec)):
        return None
    if float(np.asarray(tvec).reshape(3)[2]) <= 0.0:
        return None
    residuals = projected.reshape(-1, 2) - image_points
    rms = float(np.sqrt(np.mean(np.sum(residuals * residuals, axis=1))))
    transform = np.eye(4, dtype=np.float64)
    transform[:3, :3] = cv2.Rodrigues(rvec)[0]
    transform[:3, 3] = np.asarray(tvec).reshape(3)
    return BoardDetection(
        camera_from_board=transform,
        marker_ids=kept_ids,
        corners=kept_corners,
        rejected=tuple(rejected),
        reprojection_rms_px=rms,
        rvec=np.asarray(rvec).reshape(3),
        tvec=np.asarray(tvec).reshape(3),
    )


def matrix_from_pose(translation: Sequence[float], quaternion_xyzw: Sequence[float]) -> np.ndarray:
    translation_array = np.asarray(translation, dtype=np.float64).reshape(3)
    quaternion = np.asarray(quaternion_xyzw, dtype=np.float64).reshape(4)
    if not np.all(np.isfinite(translation_array)) or not np.all(np.isfinite(quaternion)):
        raise CalibrationError("pose contains a non-finite value")
    norm = float(np.linalg.norm(quaternion))
    if norm < 1e-9:
        raise CalibrationError("pose quaternion has zero length")
    x, y, z, w = quaternion / norm
    rotation = np.array(
        [
            [1 - 2 * (y * y + z * z), 2 * (x * y - z * w), 2 * (x * z + y * w)],
            [2 * (x * y + z * w), 1 - 2 * (x * x + z * z), 2 * (y * z - x * w)],
            [2 * (x * z - y * w), 2 * (y * z + x * w), 1 - 2 * (x * x + y * y)],
        ],
        dtype=np.float64,
    )
    transform = np.eye(4, dtype=np.float64)
    transform[:3, :3] = rotation
    transform[:3, 3] = translation_array
    return transform


def quaternion_from_matrix(transform: np.ndarray) -> np.ndarray:
    rotation = np.asarray(transform, dtype=np.float64)[:3, :3]
    trace = float(np.trace(rotation))
    if trace > 0.0:
        scale = math.sqrt(trace + 1.0) * 2.0
        quaternion = np.array(
            [
                (rotation[2, 1] - rotation[1, 2]) / scale,
                (rotation[0, 2] - rotation[2, 0]) / scale,
                (rotation[1, 0] - rotation[0, 1]) / scale,
                0.25 * scale,
            ]
        )
    else:
        index = int(np.argmax(np.diag(rotation)))
        if index == 0:
            scale = math.sqrt(1.0 + rotation[0, 0] - rotation[1, 1] - rotation[2, 2]) * 2.0
            quaternion = np.array(
                [0.25 * scale, (rotation[0, 1] + rotation[1, 0]) / scale,
                 (rotation[0, 2] + rotation[2, 0]) / scale,
                 (rotation[2, 1] - rotation[1, 2]) / scale]
            )
        elif index == 1:
            scale = math.sqrt(1.0 + rotation[1, 1] - rotation[0, 0] - rotation[2, 2]) * 2.0
            quaternion = np.array(
                [(rotation[0, 1] + rotation[1, 0]) / scale, 0.25 * scale,
                 (rotation[1, 2] + rotation[2, 1]) / scale,
                 (rotation[0, 2] - rotation[2, 0]) / scale]
            )
        else:
            scale = math.sqrt(1.0 + rotation[2, 2] - rotation[0, 0] - rotation[1, 1]) * 2.0
            quaternion = np.array(
                [(rotation[0, 2] + rotation[2, 0]) / scale,
                 (rotation[1, 2] + rotation[2, 1]) / scale, 0.25 * scale,
                 (rotation[1, 0] - rotation[0, 1]) / scale]
            )
    quaternion /= np.linalg.norm(quaternion)
    if quaternion[3] < 0.0:
        quaternion = -quaternion
    return quaternion


def pose_dict(transform: np.ndarray) -> dict[str, list[float]]:
    matrix = np.asarray(transform, dtype=np.float64).reshape(4, 4)
    return {
        "translation_m": [float(value) for value in matrix[:3, 3]],
        "rotation_xyzw": [float(value) for value in quaternion_from_matrix(matrix)],
    }


def transform_from_dict(document: Mapping[str, Any]) -> np.ndarray:
    translation = document.get("translation_m", document.get("translation"))
    return matrix_from_pose(translation, document["rotation_xyzw"])


def invert_transform(transform: np.ndarray) -> np.ndarray:
    source = np.asarray(transform, dtype=np.float64).reshape(4, 4)
    result = np.eye(4, dtype=np.float64)
    result[:3, :3] = source[:3, :3].T
    result[:3, 3] = -result[:3, :3] @ source[:3, 3]
    return result


def rotation_error_deg(first: np.ndarray, second: np.ndarray) -> float:
    relative = np.asarray(first)[:3, :3].T @ np.asarray(second)[:3, :3]
    cosine = float(np.clip((np.trace(relative) - 1.0) * 0.5, -1.0, 1.0))
    return math.degrees(math.acos(cosine))


def average_transform(transforms: Sequence[np.ndarray]) -> np.ndarray:
    if not transforms:
        raise CalibrationError("cannot average an empty transform list")
    translations = np.stack([np.asarray(item)[:3, 3] for item in transforms])
    quaternions = [quaternion_from_matrix(item) for item in transforms]
    reference = quaternions[0]
    aligned = [(-q if float(np.dot(q, reference)) < 0.0 else q) for q in quaternions]
    accumulator = sum(np.outer(q, q) for q in aligned)
    eigenvalues, eigenvectors = np.linalg.eigh(accumulator)
    quaternion = eigenvectors[:, int(np.argmax(eigenvalues))]
    if quaternion[3] < 0.0:
        quaternion = -quaternion
    return matrix_from_pose(np.mean(translations, axis=0), quaternion)


def transform_errors(
    transforms: Sequence[np.ndarray], reference: np.ndarray
) -> tuple[list[float], list[float]]:
    translation_errors = [
        float(np.linalg.norm(item[:3, 3] - reference[:3, 3])) for item in transforms
    ]
    rotation_errors = [rotation_error_deg(reference, item) for item in transforms]
    return translation_errors, rotation_errors


def rms(values: Sequence[float]) -> float:
    if not values:
        return float("nan")
    array = np.asarray(values, dtype=np.float64)
    return float(np.sqrt(np.mean(array * array)))


def _method_constant(method: str) -> int:
    return int(getattr(cv2, f"CALIB_HAND_EYE_{method}"))


def _solve_once(
    base_from_grippers: Sequence[np.ndarray], camera_from_boards: Sequence[np.ndarray], method: str
) -> np.ndarray:
    # OpenCV calibrateHandEye is formulated for eye-in-hand.  For eye-to-hand,
    # passing gripper<-base (the inverse robot poses) makes its returned
    # camera->gripper transform equal to base<-camera.  Synthetic tests lock
    # this convention down.
    gripper_from_bases = [invert_transform(item) for item in base_from_grippers]
    rotations_robot = [item[:3, :3] for item in gripper_from_bases]
    translations_robot = [item[:3, 3].reshape(3, 1) for item in gripper_from_bases]
    rotations_board = [item[:3, :3] for item in camera_from_boards]
    translations_board = [item[:3, 3].reshape(3, 1) for item in camera_from_boards]
    rotation, translation = cv2.calibrateHandEye(
        rotations_robot,
        translations_robot,
        rotations_board,
        translations_board,
        method=_method_constant(method),
    )
    result = np.eye(4, dtype=np.float64)
    result[:3, :3] = rotation
    result[:3, 3] = np.asarray(translation).reshape(3)
    if not np.all(np.isfinite(result)):
        raise CalibrationError(
            "hand-eye solver returned non-finite values; collect poses with more rotation diversity"
        )
    return result


def motion_span(base_from_grippers: Sequence[np.ndarray]) -> tuple[float, float]:
    if len(base_from_grippers) < 2:
        return 0.0, 0.0
    translation_span = 0.0
    rotation_span = 0.0
    for index, first in enumerate(base_from_grippers):
        for second in base_from_grippers[index + 1 :]:
            translation_span = max(
                translation_span, float(np.linalg.norm(first[:3, 3] - second[:3, 3]))
            )
            rotation_span = max(rotation_span, rotation_error_deg(first, second))
    return translation_span, rotation_span


def solve_eye_to_hand(
    samples: Sequence[Mapping[str, Any]], solve_config: Mapping[str, Any]
) -> SolveResult:
    minimum_samples = int(solve_config["minimum_samples"])
    if len(samples) < minimum_samples:
        raise CalibrationError(
            f"need at least {minimum_samples} samples, but dataset contains {len(samples)}"
        )
    base_from_grippers = [transform_from_dict(sample["base_from_gripper"]) for sample in samples]
    camera_from_boards = [transform_from_dict(sample["camera_from_board"]) for sample in samples]
    active = list(range(len(samples)))
    rejected: list[int] = []

    for _ in range(int(solve_config["maximum_outlier_iterations"]) + 1):
        selected_robot = [base_from_grippers[index] for index in active]
        selected_board = [camera_from_boards[index] for index in active]
        base_from_camera = _solve_once(selected_robot, selected_board, str(solve_config["method"]))
        mounts = [
            invert_transform(robot) @ base_from_camera @ board
            for robot, board in zip(selected_robot, selected_board)
        ]
        mean_mount = average_transform(mounts)
        translation_errors, rotation_errors = transform_errors(mounts, mean_mount)
        outliers = [
            local_index
            for local_index, (translation_error, rotation_error) in enumerate(
                zip(translation_errors, rotation_errors)
            )
            if translation_error > float(solve_config["outlier_translation_m"])
            or rotation_error > float(solve_config["outlier_rotation_deg"])
        ]
        if not outliers:
            break
        removable = len(active) - minimum_samples
        if removable <= 0:
            break
        ranked = sorted(
            outliers,
            key=lambda local_index: max(
                translation_errors[local_index] / float(solve_config["outlier_translation_m"]),
                rotation_errors[local_index] / float(solve_config["outlier_rotation_deg"]),
            ),
            reverse=True,
        )[:removable]
        for local_index in sorted(ranked, reverse=True):
            rejected.append(active.pop(local_index))

    selected_robot = [base_from_grippers[index] for index in active]
    selected_board = [camera_from_boards[index] for index in active]
    base_from_camera = _solve_once(selected_robot, selected_board, str(solve_config["method"]))
    mounts = [
        invert_transform(robot) @ base_from_camera @ board
        for robot, board in zip(selected_robot, selected_board)
    ]
    mean_mount = average_transform(mounts)
    translation_errors, rotation_errors = transform_errors(mounts, mean_mount)
    translation_span, rotation_span = motion_span(selected_robot)
    reprojection = [float(samples[index]["reprojection_rms_px"]) for index in active]
    metrics = {
        "sample_count": float(len(samples)),
        "used_sample_count": float(len(active)),
        "rejected_sample_count": float(len(rejected)),
        "translation_span_m": translation_span,
        "rotation_span_deg": rotation_span,
        "training_translation_rms_m": rms(translation_errors),
        "training_translation_max_m": max(translation_errors),
        "training_rotation_rms_deg": rms(rotation_errors),
        "training_rotation_max_deg": max(rotation_errors),
        "mean_marker_reprojection_rms_px": float(np.mean(reprojection)),
        "max_marker_reprojection_rms_px": max(reprojection),
    }
    return SolveResult(
        base_from_camera=base_from_camera,
        gripper_from_board=mean_mount,
        used_indices=tuple(active),
        rejected_indices=tuple(sorted(rejected)),
        translation_errors_m=tuple(translation_errors),
        rotation_errors_deg=tuple(rotation_errors),
        metrics=metrics,
    )


def training_quality(metrics: Mapping[str, float], quality: Mapping[str, Any]) -> tuple[bool, list[str]]:
    failures: list[str] = []
    checks = (
        (metrics["translation_span_m"] >= float(quality["minimum_translation_span_m"]),
         "translation span is too small"),
        (metrics["rotation_span_deg"] >= float(quality["minimum_rotation_span_deg"]),
         "rotation span is too small"),
        (metrics["training_translation_rms_m"] <= float(quality["maximum_training_translation_rms_m"]),
         "training translation RMS is too high"),
        (metrics["training_rotation_rms_deg"] <= float(quality["maximum_training_rotation_rms_deg"]),
         "training rotation RMS is too high"),
    )
    failures.extend(message for passed, message in checks if not passed)
    return not failures, failures


def validation_metrics(
    samples: Sequence[Mapping[str, Any]], base_from_camera: np.ndarray, gripper_from_board: np.ndarray
) -> dict[str, float]:
    if not samples:
        raise CalibrationError("validation dataset contains no samples")
    inferred_mounts = [
        invert_transform(transform_from_dict(sample["base_from_gripper"]))
        @ base_from_camera
        @ transform_from_dict(sample["camera_from_board"])
        for sample in samples
    ]
    translation_errors, rotation_errors = transform_errors(inferred_mounts, gripper_from_board)
    return {
        "sample_count": float(len(samples)),
        "validation_translation_rms_m": rms(translation_errors),
        "validation_translation_max_m": max(translation_errors),
        "validation_rotation_rms_deg": rms(rotation_errors),
        "validation_rotation_max_deg": max(rotation_errors),
        "mean_marker_reprojection_rms_px": float(
            np.mean([float(sample["reprojection_rms_px"]) for sample in samples])
        ),
    }


def validation_quality(metrics: Mapping[str, float], quality: Mapping[str, Any]) -> tuple[bool, list[str]]:
    failures: list[str] = []
    if metrics["sample_count"] < float(quality["minimum_validation_samples"]):
        failures.append("validation sample count is too small")
    if metrics["validation_translation_rms_m"] > float(
        quality["maximum_validation_translation_rms_m"]
    ):
        failures.append("validation translation RMS is too high")
    if metrics["validation_rotation_rms_deg"] > float(
        quality["maximum_validation_rotation_rms_deg"]
    ):
        failures.append("validation rotation RMS is too high")
    return not failures, failures


def load_json(path: Path) -> dict[str, Any]:
    if not path.is_file():
        raise CalibrationError(f"dataset does not exist: {path}")
    with path.open(encoding="utf-8") as stream:
        value = json.load(stream)
    if not isinstance(value, dict) or value.get("schema_version") != SCHEMA_VERSION:
        raise CalibrationError(f"invalid or unsupported dataset: {path}")
    if not isinstance(value.get("samples"), list):
        raise CalibrationError(f"dataset has no sample list: {path}")
    return value


def atomic_write_json(path: Path, document: Mapping[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".tmp")
    with temporary.open("w", encoding="utf-8") as stream:
        json.dump(document, stream, ensure_ascii=False, indent=2)
        stream.write("\n")
    os.replace(temporary, path)


def load_yaml(path: Path) -> dict[str, Any]:
    if not path.is_file():
        raise CalibrationError(f"result does not exist: {path}")
    with path.open(encoding="utf-8") as stream:
        value = yaml.safe_load(stream)
    if not isinstance(value, dict):
        raise CalibrationError(f"YAML root must be a mapping: {path}")
    return value


def atomic_write_yaml(path: Path, document: Mapping[str, Any]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    temporary = path.with_suffix(path.suffix + ".tmp")
    with temporary.open("w", encoding="utf-8") as stream:
        yaml.safe_dump(dict(document), stream, allow_unicode=True, sort_keys=False)
    os.replace(temporary, path)


def board_snapshot(board: Mapping[str, Any]) -> dict[str, Any]:
    return {
        "dictionary": str(board["dictionary"]),
        "markers_x": int(board["markers_x"]),
        "markers_y": int(board["markers_y"]),
        "marker_length_m": float(board["marker_length_m"]),
        "marker_separation_m": float(board["marker_separation_m"]),
        "first_marker_id": int(board["first_marker_id"]),
    }


def camera_snapshot(info: Any) -> dict[str, Any]:
    return {
        "frame_id": str(info.header.frame_id),
        "width": int(info.width),
        "height": int(info.height),
        "distortion_model": str(info.distortion_model),
        "k": [float(value) for value in info.k],
        "d": [float(value) for value in info.d],
    }


def compatible_camera(first: Mapping[str, Any], second: Mapping[str, Any]) -> bool:
    scalar_keys = ("frame_id", "width", "height", "distortion_model")
    if any(first.get(key) != second.get(key) for key in scalar_keys):
        return False
    return bool(
        np.allclose(first.get("k"), second.get("k"), rtol=0.0, atol=1e-10)
        and np.allclose(first.get("d"), second.get("d"), rtol=0.0, atol=1e-10)
    )


def rounded_matrix(transform: np.ndarray, digits: int = 12) -> list[list[float]]:
    return [[round(float(value), digits) for value in row] for row in np.asarray(transform)]


def generate_board_png(board_config: Mapping[str, Any], output: Path, dpi: int = 600) -> None:
    """Generate an A4 reference board; existing photographed boards need not use it."""
    from PIL import Image

    _, board = make_board(board_config)
    pixels_per_metre = dpi / 0.0254
    board_width_m = (
        int(board_config["markers_x"]) * float(board_config["marker_length_m"])
        + (int(board_config["markers_x"]) - 1) * float(board_config["marker_separation_m"])
    )
    board_height_m = (
        int(board_config["markers_y"]) * float(board_config["marker_length_m"])
        + (int(board_config["markers_y"]) - 1) * float(board_config["marker_separation_m"])
    )
    page_size = (round(0.210 * pixels_per_metre), round(0.297 * pixels_per_metre))
    board_size = (round(board_width_m * pixels_per_metre), round(board_height_m * pixels_per_metre))
    if board_size[0] > page_size[0] or board_size[1] > page_size[1]:
        raise CalibrationError("configured board is larger than an A4 page")
    board_image = board.draw(board_size, marginSize=0, borderBits=1)
    page = np.full((page_size[1], page_size[0]), 255, dtype=np.uint8)
    x = (page_size[0] - board_size[0]) // 2
    y = (page_size[1] - board_size[1]) // 2
    page[y : y + board_size[1], x : x + board_size[0]] = board_image
    output.parent.mkdir(parents=True, exist_ok=True)
    Image.fromarray(page).save(output, dpi=(dpi, dpi))
