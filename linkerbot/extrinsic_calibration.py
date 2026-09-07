"""Fixed-camera geometry, quality gates, and calibration file contracts."""

from __future__ import annotations

import math
from collections import deque
from typing import Mapping

import cv2
import numpy as np

from linkerbot.calibration_board import board_points
from linkerbot.runtime import ConfigurationError


def finite_number(mapping: Mapping, key: str) -> float:
    value = mapping.get(key)
    if isinstance(value, bool) or not isinstance(value, (int, float)):
        raise ConfigurationError(f"'{key}' must be a measured numeric value, not {value!r}")
    if not math.isfinite(value):
        raise ConfigurationError(f"'{key}' must be finite")
    return float(value)


def validate_config(config: Mapping) -> None:
    for section in ("frames", "topics", "board", "fixture", "robot", "quality"):
        if not isinstance(config.get(section), dict):
            raise ConfigurationError(f"Missing configuration mapping: {section}")
    frames = config["frames"]
    for key in ("base", "optical", "camera_root", "flange"):
        if not isinstance(frames.get(key), str) or not frames[key] or frames[key].startswith("/"):
            raise ConfigurationError(f"frames.{key} must be a nonempty TF frame without leading /")
    if len(set(frames.values())) != len(frames):
        raise ConfigurationError("Base, flange, camera root, and optical frames must be distinct")
    for key in ("color", "camera_info"):
        if not isinstance(config["topics"].get(key), str) or not config["topics"][key]:
            raise ConfigurationError(f"Missing topics.{key}")
    object_points = board_points(config["board"])
    quality = config["quality"]
    for key in ("min_markers", "stable_samples"):
        if type(quality.get(key)) is not int:
            raise ConfigurationError(f"quality.{key} must be an integer")
    if not 2 <= quality["min_markers"] <= len(object_points):
        raise ConfigurationError("min_markers must be between 2 and the board marker count")
    if quality["stable_samples"] < 3:
        raise ConfigurationError("stable_samples must be at least 3")
    for key in ("min_marker_side_px", "max_reprojection_rms_px",
                "ambiguity_error_margin_px", "ambiguity_angle_deg",
                "max_translation_spread_m", "max_rotation_spread_deg",
                "max_image_age_s", "capture_timeout_s"):
        if finite_number(quality, key) <= 0:
            raise ConfigurationError(f"quality.{key} must be positive")
    if not isinstance(config.get("output_directory"), str) or not config["output_directory"]:
        raise ConfigurationError("output_directory must be a nonempty path")
    board_from_flange(config["fixture"])


def board_from_flange(fixture: Mapping) -> np.ndarray:
    """Pose of the exposed R8 face when it coincides with the printed template."""
    origin = np.asarray(fixture.get("flange_origin_in_board_m"), dtype=float)
    if origin.shape != (3,) or not np.isfinite(origin).all():
        raise ConfigurationError("fixture.flange_origin_in_board_m must be a finite 3-vector")
    if (finite_number(fixture, "flange_yaw_in_board_deg") != 0
            or finite_number(fixture, "contact_z_in_flange_m") != 0 or abs(origin[2]) > 1e-9):
        raise ConfigurationError("This template requires z8=0 flush contact, +X8 right and +Y8 up")
    transform = np.eye(4)
    transform[:3, 3] = origin
    return transform


def base_from_board(config: Mapping, base_flange: np.ndarray) -> np.ndarray:
    return base_flange @ invert_transform(board_from_flange(config["fixture"]))


def invert_transform(transform: np.ndarray) -> np.ndarray:
    inverse = np.eye(4)
    inverse[:3, :3] = transform[:3, :3].T
    inverse[:3, 3] = -inverse[:3, :3] @ transform[:3, 3]
    return inverse


def rotation_distance_deg(first: np.ndarray, second: np.ndarray) -> float:
    cosine = (np.trace(first.T @ second) - 1.0) / 2.0
    return math.degrees(math.acos(float(np.clip(cosine, -1.0, 1.0))))


def quaternion_xyzw(rotation: np.ndarray) -> list[float]:
    vector, _ = cv2.Rodrigues(rotation)
    angle = float(np.linalg.norm(vector))
    if angle < 1e-12:
        return [0.0, 0.0, 0.0, 1.0]
    xyz = vector.reshape(3) * (math.sin(angle / 2.0) / angle)
    return [*xyz.tolist(), math.cos(angle / 2.0)]


def camera_model(document: Mapping, image_shape: tuple) -> tuple[np.ndarray, np.ndarray]:
    """Read ROS calibration YAML matching a raw (not rectified) color image."""
    try:
        matrix = np.asarray(document["camera_matrix"]["data"], dtype=np.float64).reshape(3, 3)
        distortion = np.asarray(document["distortion_coefficients"]["data"], dtype=np.float64).reshape(-1)
        width, height = document["image_width"], document["image_height"]
    except (KeyError, ValueError, TypeError) as error:
        raise ConfigurationError(f"Invalid CameraInfo document: {error}") from error
    if (height, width) != tuple(image_shape[:2]):
        raise ConfigurationError("CameraInfo resolution differs from image; use the actual color profile")
    if document.get("distortion_model") not in ("plumb_bob", "rational_polynomial"):
        raise ConfigurationError("Only plumb_bob/rational_polynomial raw color CameraInfo is supported")
    if (distortion.size not in (4, 5, 8, 12, 14) or not np.isfinite(matrix).all()
            or not np.isfinite(distortion).all() or matrix[0, 0] <= 0 or matrix[1, 1] <= 0
            or not np.allclose(matrix[2], [0, 0, 1])):
        raise ConfigurationError("Invalid focal lengths, distortion coefficients, or intrinsic matrix")
    return matrix, distortion


def detect_board(image: np.ndarray, board: Mapping) -> tuple[list[int], np.ndarray, np.ndarray]:
    """Return ID-sorted marker corners in board metres and raw image pixels."""
    objects = board_points(board)
    dictionary = cv2.aruco.getPredefinedDictionary(getattr(cv2.aruco, board["dictionary"]))
    if hasattr(cv2.aruco, "ArucoDetector"):
        parameters = cv2.aruco.DetectorParameters()
        parameters.cornerRefinementMethod = cv2.aruco.CORNER_REFINE_SUBPIX
        corners, ids, _ = cv2.aruco.ArucoDetector(dictionary, parameters).detectMarkers(image)
    else:
        parameters = cv2.aruco.DetectorParameters_create()
        parameters.cornerRefinementMethod = cv2.aruco.CORNER_REFINE_SUBPIX
        corners, ids, _ = cv2.aruco.detectMarkers(image, dictionary, parameters=parameters)
    if ids is None:
        raise ValueError("No board markers detected")
    found = {}
    for marker_id, points in zip(ids.flatten(), corners):
        marker_id = int(marker_id)
        if marker_id in objects:
            if marker_id in found:
                raise ValueError(f"Duplicate marker ID {marker_id}; remove duplicate printed boards")
            found[marker_id] = np.asarray(points, dtype=np.float64).reshape(4, 2)
    if not found:
        raise ValueError("Detected markers do not belong to the configured board")
    ordered_ids = sorted(found)
    return (ordered_ids, np.concatenate([objects[i] for i in ordered_ids]).astype(np.float64),
            np.concatenate([found[i] for i in ordered_ids]))


def estimate_pose(objects: np.ndarray, pixels: np.ndarray, matrix: np.ndarray,
                  distortion: np.ndarray, quality: Mapping) -> dict:
    """Solve planar PnP, rejecting inaccurate or ambiguous front-face poses."""
    objects = np.ascontiguousarray(objects, dtype=np.float64).reshape(-1, 3)
    pixels = np.ascontiguousarray(pixels, dtype=np.float64).reshape(-1, 2)
    if len(objects) != len(pixels) or len(objects) % 4 or len(objects) < quality["min_markers"] * 4:
        raise ValueError("Too few complete board markers")
    if not np.isfinite(objects).all() or not np.isfinite(pixels).all():
        raise ValueError("Nonfinite corner coordinates")
    quads = pixels.reshape(-1, 4, 2)
    sides = np.linalg.norm(quads - np.roll(quads, -1, axis=1), axis=2)
    if float(np.min(sides)) < quality["min_marker_side_px"]:
        raise ValueError("Markers too small in the image; increase color resolution or move board closer")
    solved = cv2.solvePnPGeneric(objects, pixels, matrix, distortion, flags=cv2.SOLVEPNP_IPPE)
    candidates = []
    for rvec, tvec in zip(solved[1], solved[2]):
        rotation, _ = cv2.Rodrigues(rvec)
        transformed = objects @ rotation.T + tvec.reshape(3)
        camera_in_board = -rotation.T @ tvec.reshape(3)
        if np.min(transformed[:, 2]) <= 0 or camera_in_board[2] <= 0:
            continue
        projected, _ = cv2.projectPoints(objects, rvec, tvec, matrix, distortion)
        rms = float(np.sqrt(np.mean(np.sum((projected.reshape(-1, 2) - pixels) ** 2, axis=1))))
        candidates.append((rms, rotation, rvec, tvec))
    if not candidates:
        raise ValueError("No front-facing positive-depth PnP solution")
    candidates.sort(key=lambda candidate: candidate[0])
    best = candidates[0]
    if len(candidates) > 1:
        second = candidates[1]
        if (second[0] - best[0] < quality["ambiguity_error_margin_px"]
                and rotation_distance_deg(best[1], second[1]) > quality["ambiguity_angle_deg"]):
            raise ValueError("Planar pose is ambiguous; use a larger board view or a more oblique view")
    rvec, tvec = cv2.solvePnPRefineLM(objects, pixels, matrix, distortion, best[2], best[3])
    rotation, _ = cv2.Rodrigues(rvec)
    transform = np.eye(4)
    transform[:3, :3], transform[:3, 3] = rotation, tvec.reshape(3)
    projected, _ = cv2.projectPoints(objects, rvec, tvec, matrix, distortion)
    rms = float(np.sqrt(np.mean(np.sum((projected.reshape(-1, 2) - pixels) ** 2, axis=1))))
    if (rms > quality["max_reprojection_rms_px"] or not np.isfinite(transform).all()
            or np.min((objects @ rotation.T + tvec.reshape(3))[:, 2]) <= 0
            or invert_transform(transform)[2, 3] <= 0):
        raise ValueError(f"Pose rejected: reprojection RMS {rms:.3f} px")
    return {"camera_from_board": transform, "rms_px": rms, "rvec": rvec, "tvec": tvec}


class StableObservations:
    """Keep consecutive unique frames with an unchanged visible marker set."""

    def __init__(self, quality: Mapping):
        self.quality = quality
        self.samples = deque(maxlen=quality["stable_samples"])
        self.ids: list[int] = []

    def clear(self) -> None:
        self.samples.clear()
        self.ids = []

    def add(self, stamp: int, ids: list[int], pixels: np.ndarray, pose: dict) -> None:
        if ids != self.ids or (self.samples and stamp < self.samples[-1][0]):
            self.clear()
        self.ids = ids
        if self.samples and stamp == self.samples[-1][0]:
            return
        self.samples.append((stamp, pixels.copy(), pose))

    def finish(self, objects: np.ndarray, matrix: np.ndarray, distortion: np.ndarray) -> tuple[dict, dict]:
        if len(self.samples) < self.quality["stable_samples"]:
            raise ValueError(f"Collecting {len(self.samples)}/{self.quality['stable_samples']} frames")
        pixels = np.median(np.stack([sample[1] for sample in self.samples]), axis=0)
        pose = estimate_pose(objects, pixels, matrix, distortion, self.quality)
        reference = invert_transform(pose["camera_from_board"])
        transforms = [invert_transform(sample[2]["camera_from_board"]) for sample in self.samples]
        translation_spread = max(float(np.linalg.norm(t[:3, 3] - reference[:3, 3])) for t in transforms)
        rotation_spread = max(rotation_distance_deg(t[:3, :3], reference[:3, :3]) for t in transforms)
        if (translation_spread > self.quality["max_translation_spread_m"]
                or rotation_spread > self.quality["max_rotation_spread_deg"]):
            raise ValueError(f"Unstable: max spread {translation_spread * 1000:.1f} mm / {rotation_spread:.2f} deg")
        return pose, {"sample_count": len(self.samples), "marker_ids": self.ids,
                      "reprojection_rms_px": pose["rms_px"],
                      "max_translation_spread_m": translation_spread,
                      "max_rotation_spread_deg": rotation_spread,
                      "first_image_stamp_ns": self.samples[0][0],
                      "last_image_stamp_ns": self.samples[-1][0]}


def transform_document(transform: np.ndarray, parent: str, child: str) -> dict:
    return {"parent_frame": parent, "child_frame": child,
            "translation_m": transform[:3, 3].tolist(),
            "quaternion_xyzw": quaternion_xyzw(transform[:3, :3]),
            "matrix": transform.tolist()}


def result_document(config: Mapping, pose: dict, statistics: dict, intrinsics: Mapping,
                    base_flange: np.ndarray, robot_observation: Mapping) -> dict:
    base_board = base_from_board(config, base_flange)
    base_camera = base_board @ invert_transform(pose["camera_from_board"])
    return {"schema_version": 1, "mount": "fixed_to_base", "units": "metres",
            "transform": transform_document(base_camera, config["frames"]["base"], config["frames"]["optical"]),
            "base_from_board": base_board.tolist(), "camera_from_board": pose["camera_from_board"].tolist(),
            "base_from_flange": base_flange.tolist(), "robot_observation": dict(robot_observation),
            "camera_root_frame": config["frames"]["camera_root"],
            "quality": statistics, "camera_info": dict(intrinsics), "configuration": dict(config)}


def saved_transform(document: Mapping) -> np.ndarray:
    """Validate a calibration result before a TF publisher can consume it."""
    if document.get("schema_version") != 1 or document.get("mount") != "fixed_to_base":
        raise ConfigurationError("Unsupported external calibration result")
    try:
        transform = np.asarray(document["transform"]["matrix"], dtype=float).reshape(4, 4)
        frames = document["configuration"]["frames"]
        if (document["transform"]["parent_frame"] != frames["base"]
                or document["transform"]["child_frame"] != frames["optical"]
                or document["camera_root_frame"] != frames["camera_root"]):
            raise ConfigurationError("Result frame names are inconsistent")
    except (KeyError, TypeError, ValueError) as error:
        raise ConfigurationError(f"Invalid saved transform: {error}") from error
    rotation = transform[:3, :3]
    if (not np.isfinite(transform).all() or not np.allclose(transform[3], [0, 0, 0, 1])
            or not np.allclose(rotation.T @ rotation, np.eye(3), atol=1e-6)
            or not np.isclose(np.linalg.det(rotation), 1.0, atol=1e-6)):
        raise ConfigurationError("Saved transform is not a rigid 4x4 transformation")
    return transform
