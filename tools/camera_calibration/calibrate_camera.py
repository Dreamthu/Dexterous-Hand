#!/usr/bin/env python3
"""Interactive ROS 2 monocular camera calibration with a chessboard."""

from __future__ import annotations

import argparse
import math
import sys
from datetime import datetime
from pathlib import Path
from typing import List, Optional, Sequence, Tuple

import cv2
import numpy as np
import rclpy
import yaml
from cv_bridge import CvBridge, CvBridgeError
from rclpy.node import Node
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import Image


WINDOW_NAME = "Gemini2 intrinsic calibration"


class ImageSubscriber(Node):
    """Keep the newest image from a ROS 2 image topic."""

    def __init__(self, topic: str) -> None:
        super().__init__("camera_intrinsic_calibrator")
        self._bridge = CvBridge()
        self.latest_frame: Optional[np.ndarray] = None
        self.frame_sequence = 0
        self.create_subscription(Image, topic, self._on_image, qos_profile_sensor_data)
        self.get_logger().info(f"Waiting for images on {topic}")

    def _on_image(self, message: Image) -> None:
        try:
            self.latest_frame = self._bridge.imgmsg_to_cv2(message, desired_encoding="bgr8")
            self.frame_sequence += 1
        except CvBridgeError as error:
            self.get_logger().error(f"Failed to convert image: {error}")


def parse_arguments() -> argparse.Namespace:
    script_dir = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser(
        description=(
            "Subscribe to a ROS 2 image topic, collect chessboard observations, "
            "and write ROS-compatible intrinsic calibration files."
        )
    )
    parser.add_argument("--topic", default="/camera/color/image_raw", help="ROS image topic")
    parser.add_argument("--camera-name", default="gemini2_color", help="Name stored in YAML")
    parser.add_argument(
        "--squares-x",
        type=int,
        default=11,
        help="number of printed squares along the board X direction (default: 11)",
    )
    parser.add_argument(
        "--squares-y",
        type=int,
        default=8,
        help="number of printed squares along the board Y direction (default: 8)",
    )
    parser.add_argument(
        "--inner-cols",
        type=int,
        default=None,
        help="override detected inner-corner columns; must be used with --inner-rows",
    )
    parser.add_argument(
        "--inner-rows",
        type=int,
        default=None,
        help="override detected inner-corner rows; must be used with --inner-cols",
    )
    parser.add_argument(
        "--square-size-mm",
        type=float,
        default=15.0,
        help="measured side length of one printed square in millimetres (default: 15)",
    )
    parser.add_argument(
        "--min-samples",
        type=int,
        default=15,
        help="minimum number of accepted observations before calibration (default: 15)",
    )
    parser.add_argument(
        "--min-board-area-ratio",
        type=float,
        default=0.05,
        help="minimum inner-corner hull area relative to the image (default: 0.05)",
    )
    parser.add_argument(
        "--output-dir",
        type=Path,
        default=script_dir / "output",
        help="parent directory for images and calibration results",
    )
    parser.add_argument(
        "--classic-detector",
        action="store_true",
        help="use classic findChessboardCorners instead of findChessboardCornersSB",
    )
    arguments = parser.parse_args()

    if (arguments.inner_cols is None) != (arguments.inner_rows is None):
        parser.error("--inner-cols and --inner-rows must be specified together")
    if arguments.squares_x < 3 or arguments.squares_y < 3:
        parser.error("the board must have at least 3 squares in each direction")
    if arguments.inner_cols is not None and (
        arguments.inner_cols < 2 or arguments.inner_rows < 2
    ):
        parser.error("the board must have at least 2 inner corners in each direction")
    if arguments.square_size_mm <= 0:
        parser.error("--square-size-mm must be positive")
    if arguments.min_samples < 3:
        parser.error("--min-samples must be at least 3")
    if not 0.0 < arguments.min_board_area_ratio < 1.0:
        parser.error("--min-board-area-ratio must be between 0 and 1")
    return arguments


def inner_corner_shape(arguments: argparse.Namespace) -> Tuple[int, int]:
    if arguments.inner_cols is not None:
        return arguments.inner_cols, arguments.inner_rows
    return arguments.squares_x - 1, arguments.squares_y - 1


def make_object_points(
    inner_cols: int, inner_rows: int, square_size_m: float
) -> np.ndarray:
    points = np.zeros((inner_cols * inner_rows, 3), dtype=np.float32)
    points[:, :2] = np.mgrid[0:inner_cols, 0:inner_rows].T.reshape(-1, 2)
    points[:, :2] *= square_size_m
    return points


def find_corners(
    gray: np.ndarray, pattern_size: Tuple[int, int], classic_detector: bool
) -> Tuple[bool, Optional[np.ndarray]]:
    if not classic_detector and hasattr(cv2, "findChessboardCornersSB"):
        flags = cv2.CALIB_CB_NORMALIZE_IMAGE
        found, corners = cv2.findChessboardCornersSB(gray, pattern_size, flags=flags)
        if found:
            return True, corners.astype(np.float32)

    flags = (
        cv2.CALIB_CB_ADAPTIVE_THRESH
        | cv2.CALIB_CB_NORMALIZE_IMAGE
        | cv2.CALIB_CB_FAST_CHECK
    )
    found, corners = cv2.findChessboardCorners(gray, pattern_size, flags=flags)
    if not found:
        return False, None

    criteria = (
        cv2.TERM_CRITERIA_EPS | cv2.TERM_CRITERIA_MAX_ITER,
        40,
        0.001,
    )
    refined = cv2.cornerSubPix(gray, corners, (11, 11), (-1, -1), criteria)
    return True, refined


def observation_metrics(corners: np.ndarray, image_size: Tuple[int, int]) -> Tuple[float, float, float]:
    width, height = image_size
    points = corners.reshape(-1, 2)
    center = points.mean(axis=0)
    hull = cv2.convexHull(points.astype(np.float32))
    area_fraction = cv2.contourArea(hull) / float(width * height)
    return center[0] / width, center[1] / height, area_fraction


def draw_status(
    frame: np.ndarray,
    corners: Optional[np.ndarray],
    pattern_size: Tuple[int, int],
    sample_count: int,
    min_samples: int,
    area_fraction: float,
    min_area_fraction: float,
) -> np.ndarray:
    display = frame.copy()
    found = corners is not None
    if found:
        cv2.drawChessboardCorners(display, pattern_size, corners, True)

    status_color = (40, 220, 40) if found else (40, 40, 240)
    status_text = "BOARD FOUND - press SPACE" if found else "Board not found"
    lines = [
        status_text,
        f"Samples: {sample_count}/{min_samples} minimum",
        f"Board area: {area_fraction:.1%} (minimum {min_area_fraction:.1%})",
        "SPACE capture | D undo | C calibrate | ESC abort",
    ]
    y = 28
    for index, line in enumerate(lines):
        color = status_color if index == 0 else (255, 255, 255)
        cv2.putText(display, line, (12, y), cv2.FONT_HERSHEY_SIMPLEX, 0.62, (0, 0, 0), 3)
        cv2.putText(display, line, (12, y), cv2.FONT_HERSHEY_SIMPLEX, 0.62, color, 1)
        y += 27
    return display


def calculate_per_view_errors(
    object_points: Sequence[np.ndarray],
    image_points: Sequence[np.ndarray],
    rvecs: Sequence[np.ndarray],
    tvecs: Sequence[np.ndarray],
    camera_matrix: np.ndarray,
    distortion: np.ndarray,
) -> List[float]:
    errors: List[float] = []
    for object_view, image_view, rvec, tvec in zip(
        object_points, image_points, rvecs, tvecs
    ):
        projected, _ = cv2.projectPoints(
            object_view, rvec, tvec, camera_matrix, distortion
        )
        delta = image_view.reshape(-1, 2) - projected.reshape(-1, 2)
        errors.append(float(np.sqrt(np.mean(np.sum(delta * delta, axis=1)))))
    return errors


def matrix_block(matrix: np.ndarray) -> dict:
    rows, cols = matrix.shape
    return {
        "rows": int(rows),
        "cols": int(cols),
        "data": [float(value) for value in matrix.reshape(-1)],
    }


def save_results(
    session_dir: Path,
    camera_name: str,
    image_size: Tuple[int, int],
    camera_matrix: np.ndarray,
    distortion: np.ndarray,
    rms: float,
    per_view_errors: Sequence[float],
    pattern_size: Tuple[int, int],
    square_size_mm: float,
    topic: str,
) -> Tuple[Path, Path]:
    width, height = image_size
    distortion_flat = distortion.reshape(-1)
    projection = np.array(
        [
            [camera_matrix[0, 0], 0.0, camera_matrix[0, 2], 0.0],
            [0.0, camera_matrix[1, 1], camera_matrix[1, 2], 0.0],
            [0.0, 0.0, 1.0, 0.0],
        ],
        dtype=np.float64,
    )
    camera_info = {
        "image_width": width,
        "image_height": height,
        "camera_name": camera_name,
        "camera_matrix": matrix_block(camera_matrix),
        "distortion_model": "plumb_bob",
        "distortion_coefficients": {
            "rows": 1,
            "cols": int(distortion_flat.size),
            "data": [float(value) for value in distortion_flat],
        },
        "rectification_matrix": matrix_block(np.eye(3, dtype=np.float64)),
        "projection_matrix": matrix_block(projection),
    }
    report = {
        "created_at": datetime.now().astimezone().isoformat(),
        "source_topic": topic,
        "image_size": {"width": width, "height": height},
        "board": {
            "inner_corner_columns": pattern_size[0],
            "inner_corner_rows": pattern_size[1],
            "square_size_mm": square_size_mm,
        },
        "sample_count": len(per_view_errors),
        "focal_length_ratio_fx_fy": float(camera_matrix[0, 0] / camera_matrix[1, 1]),
        "rms_reprojection_error_px": float(rms),
        "mean_per_view_error_px": float(np.mean(per_view_errors)),
        "max_per_view_error_px": float(np.max(per_view_errors)),
        "per_view_error_px": [float(value) for value in per_view_errors],
    }

    calibration_path = session_dir / "camera_info.yaml"
    report_path = session_dir / "calibration_report.yaml"
    with calibration_path.open("w", encoding="utf-8") as stream:
        yaml.safe_dump(camera_info, stream, sort_keys=False, allow_unicode=True)
    with report_path.open("w", encoding="utf-8") as stream:
        yaml.safe_dump(report, stream, sort_keys=False, allow_unicode=True)
    return calibration_path, report_path


def calibrate_and_save(
    session_dir: Path,
    camera_name: str,
    image_size: Tuple[int, int],
    object_points: Sequence[np.ndarray],
    image_points: Sequence[np.ndarray],
    pattern_size: Tuple[int, int],
    square_size_mm: float,
    topic: str,
) -> bool:
    print("\nCalibrating. This may take a few seconds...")
    rms, camera_matrix, distortion, rvecs, tvecs = cv2.calibrateCamera(
        object_points,
        image_points,
        image_size,
        None,
        None,
    )
    if not math.isfinite(rms):
        print("Calibration failed: OpenCV returned a non-finite error.", file=sys.stderr)
        return False

    errors = calculate_per_view_errors(
        object_points,
        image_points,
        rvecs,
        tvecs,
        camera_matrix,
        distortion,
    )
    calibration_path, report_path = save_results(
        session_dir,
        camera_name,
        image_size,
        camera_matrix,
        distortion,
        rms,
        errors,
        pattern_size,
        square_size_mm,
        topic,
    )

    print(f"RMS reprojection error: {rms:.4f} px")
    print(f"Mean per-view error:    {np.mean(errors):.4f} px")
    print(f"Worst per-view error:   {np.max(errors):.4f} px")
    focal_ratio = float(camera_matrix[0, 0] / camera_matrix[1, 1])
    print(f"Focal ratio fx/fy:      {focal_ratio:.4f}")
    print(f"CameraInfo YAML: {calibration_path}")
    print(f"Quality report:  {report_path}")
    if rms > 1.0:
        print(
            "WARNING: RMS is above 1 px. Retake sharper, more varied views and verify "
            "the printed square size."
        )
    if not 0.9 <= focal_ratio <= 1.1:
        print(
            "WARNING: fx/fy is far from 1.0. The views may not constrain the model; "
            "use a larger, rigid board with more tilted poses."
        )
    return True


def main() -> int:
    arguments = parse_arguments()
    pattern_size = inner_corner_shape(arguments)
    square_size_m = arguments.square_size_mm / 1000.0
    object_template = make_object_points(*pattern_size, square_size_m)

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    session_dir = arguments.output_dir.expanduser().resolve() / timestamp
    images_dir = session_dir / "images"
    images_dir.mkdir(parents=True, exist_ok=False)

    print(f"Printed squares: {arguments.squares_x} x {arguments.squares_y}")
    print(f"Detecting inner corners: {pattern_size[0]} x {pattern_size[1]}")
    print(f"Square size: {arguments.square_size_mm:.3f} mm")
    print(f"Session directory: {session_dir}")
    print("Use 15-30 sharp views covering the centre, edges, corners and tilted poses.")

    rclpy.init()
    node = ImageSubscriber(arguments.topic)
    object_points: List[np.ndarray] = []
    image_points: List[np.ndarray] = []
    sample_paths: List[Path] = []
    image_size: Optional[Tuple[int, int]] = None
    last_sequence = -1
    corners: Optional[np.ndarray] = None
    frame: Optional[np.ndarray] = None
    area_fraction = 0.0

    try:
        cv2.namedWindow(WINDOW_NAME, cv2.WINDOW_NORMAL)
        while rclpy.ok():
            rclpy.spin_once(node, timeout_sec=0.03)
            if node.latest_frame is None:
                key = cv2.waitKey(10) & 0xFF
                if key == 27:
                    return 1
                continue

            if node.frame_sequence != last_sequence:
                frame = node.latest_frame.copy()
                last_sequence = node.frame_sequence
                current_size = (frame.shape[1], frame.shape[0])
                if image_size is not None and current_size != image_size:
                    print(
                        f"Ignoring changed image size {current_size}; expected {image_size}.",
                        file=sys.stderr,
                    )
                    continue
                gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
                found, detected = find_corners(
                    gray, pattern_size, arguments.classic_detector
                )
                corners = detected if found else None
                if corners is not None:
                    _, _, area_fraction = observation_metrics(corners, current_size)
                else:
                    area_fraction = 0.0
                display = draw_status(
                    frame,
                    corners,
                    pattern_size,
                    len(image_points),
                    arguments.min_samples,
                    area_fraction,
                    arguments.min_board_area_ratio,
                )
                cv2.imshow(WINDOW_NAME, display)

            key = cv2.waitKey(1) & 0xFF
            if key == 27:
                print("Calibration aborted; captured images were kept in the session directory.")
                return 1
            if key in (ord("d"), ord("D")):
                if image_points:
                    object_points.pop()
                    image_points.pop()
                    removed_path = sample_paths.pop()
                    removed_path.unlink(missing_ok=True)
                    print(f"Removed last sample. Remaining: {len(image_points)}")
                continue
            if key == ord(" "):
                if frame is None or corners is None:
                    print("Board is not detected; sample not captured.")
                    continue
                current_size = (frame.shape[1], frame.shape[0])
                if area_fraction < arguments.min_board_area_ratio:
                    print(
                        f"Board covers only {area_fraction:.1%} of the image; "
                        f"move it closer (minimum {arguments.min_board_area_ratio:.1%})."
                    )
                    continue
                if image_size is None:
                    image_size = current_size
                sample_number = len(image_points) + 1
                sample_path = images_dir / f"sample_{sample_number:03d}.png"
                if not cv2.imwrite(str(sample_path), frame):
                    print(f"Failed to write {sample_path}", file=sys.stderr)
                    continue
                object_points.append(object_template.copy())
                image_points.append(corners.copy())
                sample_paths.append(sample_path)
                cx, cy, area = observation_metrics(corners, current_size)
                sharpness = cv2.Laplacian(
                    cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY), cv2.CV_64F
                ).var()
                print(
                    f"Captured {sample_number:02d}: center=({cx:.2f}, {cy:.2f}), "
                    f"board_area={area:.1%}, sharpness={sharpness:.1f}"
                )
                continue
            if key in (ord("c"), ord("C")):
                if len(image_points) < arguments.min_samples:
                    print(
                        f"Need at least {arguments.min_samples} samples; "
                        f"currently have {len(image_points)}."
                    )
                    continue
                assert image_size is not None
                return 0 if calibrate_and_save(
                    session_dir,
                    arguments.camera_name,
                    image_size,
                    object_points,
                    image_points,
                    pattern_size,
                    arguments.square_size_mm,
                    arguments.topic,
                ) else 2
    except KeyboardInterrupt:
        print("\nInterrupted; captured images were kept in the session directory.")
        return 130
    finally:
        cv2.destroyAllWindows()
        node.destroy_node()
        if rclpy.ok():
            rclpy.shutdown()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
