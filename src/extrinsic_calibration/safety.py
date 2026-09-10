"""ROS-free capture contracts and TF composition checks."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any, Mapping, Sequence

import numpy as np

from .core import CalibrationError, invert_transform, motion_span


def robot_contract(config: Mapping[str, Any]) -> dict[str, Any]:
    robot = config.get("robot")
    if robot is None:
        raise CalibrationError("missing robot tool contract; use the current configuration for new captures")
    return {key: robot[key] for key in (
        "model_end_link", "expected_tool_name", "expected_tool_translation_m",
        "expected_tool_euler_rad", "tool_translation_tolerance_m", "tool_euler_tolerance_rad",
    )}


def validate_tool_snapshot(snapshot: Mapping[str, Any], robot: Mapping[str, Any]) -> None:
    if snapshot["name"] != robot["expected_tool_name"]:
        raise CalibrationError(f"tool is {snapshot['name']!r}, expected {robot['expected_tool_name']!r}")
    for field, expected, tolerance in (
        ("translation_m", "expected_tool_translation_m", "tool_translation_tolerance_m"),
        ("euler_rad", "expected_tool_euler_rad", "tool_euler_tolerance_rad"),
    ):
        values = np.asarray(snapshot[field], dtype=float)
        if values.shape != (3,) or not np.all(np.isfinite(values)):
            raise CalibrationError(f"invalid tool {field}")
        # Compare the controller's raw Euler representation conservatively, without
        # guessing a convention or silently accepting a different configured frame.
        if np.max(np.abs(values - np.asarray(robot[expected], dtype=float))) > float(robot[tolerance]):
            raise CalibrationError(f"tool {field} differs from the configured fixed offset")


@dataclass(frozen=True)
class Stationarity:
    passed: bool
    reason: str
    translation_span_m: float = float("inf")
    rotation_span_deg: float = float("inf")
    sample_count: int = 0
    coverage_s: float = 0.0


def stationary_window(
    poses: Sequence[tuple[float, str, np.ndarray]], image_time: float,
    base_frame: str, capture: Mapping[str, Any],
    paired_pose_time: float | None = None,
) -> Stationarity:
    """Require a continuous window bracketed at its start, not merely three poses."""
    window = float(capture["stationary_window_s"])
    max_gap = float(capture["maximum_pose_gap_s"])
    # The nearest paired pose can be just AFTER the image. Include it as well,
    # otherwise motion starting at exposure time could escape the window check.
    end_time = max(image_time, paired_pose_time if paired_pose_time is not None else image_time)
    eligible = [entry for entry in poses if entry[0] <= end_time]
    if len(eligible) < 3:
        return Stationarity(False, "insufficient pose history")
    times = np.asarray([entry[0] for entry in eligible])
    if not np.all(np.isfinite(times)) or np.any(np.diff(times) <= 0):
        return Stationarity(False, "duplicate or backwards pose timestamps")
    boundary = image_time - window
    preceding = np.flatnonzero(times <= boundary)
    if len(preceding) == 0:
        return Stationarity(False, "pose history does not cover the stationary window")
    selected = eligible[int(preceding[-1]):]
    times = np.asarray([entry[0] for entry in selected])
    if (len(selected) < 3 or boundary - times[0] > max_gap
            or end_time - times[-1] > max_gap or np.any(np.diff(times) > max_gap)):
        return Stationarity(False, "pose history has a gap or its latest timestamp is stale")
    if any(entry[1] != base_frame for entry in selected):
        return Stationarity(False, "parent frame changed or differs from the configured base")
    translation, rotation = motion_span([entry[2] for entry in selected])
    passed = (translation <= float(capture["maximum_stationary_translation_m"])
              and rotation <= float(capture["maximum_stationary_rotation_deg"]))
    return Stationarity(passed, "OK" if passed else "robot is moving", translation, rotation,
                        len(selected), float(times[-1] - times[0]))


def validate_camera_tf_tree(
    parents: Mapping[str, set[str]], static_edges: set[tuple[str, str]],
    root: str, optical: str, base: str,
) -> None:
    if len({root, optical, base}) != 3:
        raise CalibrationError("base, camera root and optical frame must be distinct")
    if parents.get(root):
        raise CalibrationError(
            f"camera root {root!r} already has parent(s) {sorted(parents[root])}; "
            "stop the existing external TF publisher before publishing a new calibration"
        )
    ancestors_to_check = [base]
    visited_ancestors: set[str] = set()
    while ancestors_to_check:
        ancestor = ancestors_to_check.pop()
        if ancestor == root:
            raise CalibrationError("attaching camera root below base would create a TF cycle")
        if ancestor not in visited_ancestors:
            visited_ancestors.add(ancestor)
            ancestors_to_check.extend(parents.get(ancestor, set()))
    child = optical
    visited: set[str] = set()
    while child != root:
        if child in visited:
            raise CalibrationError("cycle in camera TF tree")
        visited.add(child)
        ancestors = parents.get(child, set())
        if len(ancestors) != 1:
            raise CalibrationError(f"camera frame {child!r} has missing or conflicting parents: {sorted(ancestors)}")
        parent = next(iter(ancestors))
        if (parent, child) not in static_edges or parent == base:
            raise CalibrationError("camera root-to-optical path must use only the driver's internal static TF")
        child = parent


def base_from_camera_root(base_from_optical: np.ndarray, root_from_optical: np.ndarray) -> np.ndarray:
    return base_from_optical @ invert_transform(root_from_optical)
