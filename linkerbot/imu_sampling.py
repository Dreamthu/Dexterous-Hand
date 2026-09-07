"""Bounded, ROS-independent IMU collection and stationary-window checks.

The Gemini 2 synchronized IMU stream provides acceleration and angular velocity
only.  This module deliberately does not derive orientation or adjust camera
extrinsics.  It answers the narrower question required during external-camera
calibration: did a timestamped image interval have enough, contiguous IMU
samples consistent with a stationary camera?
"""

from __future__ import annotations

from collections import deque
from collections.abc import Mapping, Sequence
import math
from numbers import Integral, Real
from typing import Any

import numpy as np


DEFAULT_SETTINGS: dict[str, Any] = {
    "window_s": 1.0,
    "min_samples": 50,
    "max_gap_s": 0.10,
    "gravity_m_s2": 9.80665,
    "max_accel_norm_error_m_s2": 0.6,
    "max_accel_std_m_s2": 0.15,
    "max_gyro_norm_rad_s": 0.035,
    "max_samples": 10_000,
}


class ImuWindow:
    """Collect SI-unit IMU samples and assess an image-capture interval.

    ``add`` returns ``True`` when a sample was retained and ``False`` when it
    was rejected.  ``last_add_reason`` records ``accepted``, ``duplicate`` or
    the rejection reason.  A valid sample that has an earlier timestamp clears
    the previous sequence, then begins a new one; this prevents clock resets
    from silently joining two unrelated windows.

    ``assess(start_ns, end_ns)`` covers the entire supplied image interval. If
    that interval is shorter than ``window_s``, its start is extended backwards
    to require at least that much stationary history. The result is a plain
    serializable dictionary with ``ready``, ``still`` and a machine-readable
    ``reason``.
    """

    def __init__(self, settings: Mapping[str, Any]):
        if not isinstance(settings, Mapping):
            raise ValueError("IMU settings must be a mapping")
        unknown = set(settings) - ({"frame_id"} | set(DEFAULT_SETTINGS))
        if unknown:
            raise ValueError(f"Unknown IMU setting(s): {', '.join(sorted(unknown))}")
        frame_id = settings.get("frame_id")
        if not isinstance(frame_id, str) or not frame_id or frame_id.startswith("/"):
            raise ValueError("imu.frame_id must be a nonempty TF frame without leading /")
        merged = {**DEFAULT_SETTINGS, **dict(settings)}
        for name in ("window_s", "max_gap_s", "gravity_m_s2", "max_accel_norm_error_m_s2",
                     "max_accel_std_m_s2", "max_gyro_norm_rad_s"):
            value = merged[name]
            if isinstance(value, bool) or not isinstance(value, Real) or not math.isfinite(float(value)):
                raise ValueError(f"imu.{name} must be a finite number")
            if float(value) <= 0:
                raise ValueError(f"imu.{name} must be positive")
            merged[name] = float(value)
        for name in ("min_samples", "max_samples"):
            value = merged[name]
            if isinstance(value, bool) or not isinstance(value, Integral) or int(value) <= 0:
                raise ValueError(f"imu.{name} must be a positive integer")
            merged[name] = int(value)
        if merged["min_samples"] > merged["max_samples"]:
            raise ValueError("imu.min_samples cannot exceed imu.max_samples")

        self.settings = merged
        self._samples: deque[tuple[int, np.ndarray, np.ndarray]] = deque(
            maxlen=merged["max_samples"]
        )
        self.last_add_reason = "empty"

    def clear(self) -> None:
        """Discard retained samples after the adapter detects a stream restart."""
        self._samples.clear()
        self.last_add_reason = "empty"

    @property
    def sample_count(self) -> int:
        """Number of accepted samples currently retained in the bounded buffer."""
        return len(self._samples)

    @staticmethod
    def _vector(value: Any) -> np.ndarray | None:
        if isinstance(value, (str, bytes)) or not isinstance(value, (Sequence, np.ndarray)):
            return None
        try:
            raw = np.asarray(value)
            if raw.shape != (3,) or raw.dtype.kind not in "iuf":
                return None
            vector = raw.astype(np.float64)
        except (TypeError, ValueError, OverflowError):
            return None
        if vector.shape != (3,) or not np.isfinite(vector).all():
            return None
        return vector

    def add(self, stamp_ns: int, frame_id: str, acceleration: Sequence[float],
            angular_velocity: Sequence[float]) -> bool:
        """Retain one synchronized IMU sample expressed in SI units.

        Invalid samples and duplicates are ignored.  This method cannot judge
        receive-time freshness because that must be measured in the ROS adapter
        against its local receipt clock.
        """
        if (isinstance(stamp_ns, bool) or not isinstance(stamp_ns, Integral)
                or int(stamp_ns) <= 0):
            self.last_add_reason = "invalid_stamp"
            return False
        if frame_id != self.settings["frame_id"]:
            self.last_add_reason = "wrong_frame"
            return False
        acceleration_vector = self._vector(acceleration)
        angular_velocity_vector = self._vector(angular_velocity)
        if acceleration_vector is None or angular_velocity_vector is None:
            self.last_add_reason = "nonfinite_or_invalid_vector"
            return False

        stamp = int(stamp_ns)
        if self._samples and stamp == self._samples[-1][0]:
            self.last_add_reason = "duplicate"
            return False
        if self._samples and stamp < self._samples[-1][0]:
            self._samples.clear()
            self.last_add_reason = "backward_stamp_reset"
        else:
            self.last_add_reason = "accepted"
        self._samples.append((stamp, acceleration_vector.copy(), angular_velocity_vector.copy()))
        return True

    def _interval(self, start_ns: int, end_ns: int) -> tuple[int, int] | None:
        if (isinstance(start_ns, bool) or isinstance(end_ns, bool)
                or not isinstance(start_ns, Integral) or not isinstance(end_ns, Integral)
                or int(start_ns) <= 0 or int(end_ns) <= 0 or int(end_ns) < int(start_ns)):
            return None
        requested_start, requested_end = int(start_ns), int(end_ns)
        lookback = int(round(self.settings["window_s"] * 1_000_000_000))
        return min(requested_start, requested_end - lookback), requested_end

    def _summary(self, *, ready: bool, still: bool, reason: str,
                 requested_start_ns: int | None, requested_end_ns: int | None,
                 window_start_ns: int | None, window_end_ns: int | None,
                 samples: list[tuple[int, np.ndarray, np.ndarray]] | None = None) -> dict[str, Any]:
        summary: dict[str, Any] = {
            "ready": bool(ready),
            "still": bool(still),
            "reason": reason,
            "requested_start_ns": requested_start_ns,
            "requested_end_ns": requested_end_ns,
            "window_start_ns": window_start_ns,
            "window_end_ns": window_end_ns,
            "sample_count": 0 if samples is None else len(samples),
            "buffered_sample_count": len(self._samples),
        }
        if samples is None or not samples:
            return summary

        acceleration = np.stack([sample[1] for sample in samples])
        angular_velocity = np.stack([sample[2] for sample in samples])
        acceleration_norm = np.linalg.norm(acceleration, axis=1)
        gyro_norm = np.linalg.norm(angular_velocity, axis=1)
        gravity = self.settings["gravity_m_s2"]
        summary.update({
            "first_sample_stamp_ns": samples[0][0],
            "last_sample_stamp_ns": samples[-1][0],
            "mean_acceleration_m_s2": acceleration.mean(axis=0).tolist(),
            "mean_angular_velocity_rad_s": angular_velocity.mean(axis=0).tolist(),
            "mean_accel_norm_m_s2": float(acceleration_norm.mean()),
            "accel_norm_mean_error_m_s2": float(abs(acceleration_norm.mean() - gravity)),
            "max_accel_norm_error_m_s2": float(np.max(abs(acceleration_norm - gravity))),
            "accel_norm_std_m_s2": float(acceleration_norm.std()),
            "accel_axis_std_m_s2": acceleration.std(axis=0).tolist(),
            "max_accel_axis_std_m_s2": float(acceleration.std(axis=0).max()),
            "max_gyro_norm_rad_s": float(gyro_norm.max()),
            "gyro_axis_std_rad_s": angular_velocity.std(axis=0).tolist(),
            "max_gyro_axis_std_rad_s": float(angular_velocity.std(axis=0).max()),
        })
        return summary

    def assess(self, start_ns: int, end_ns: int) -> dict[str, Any]:
        """Assess contiguous stationary IMU coverage for an image interval.

        The window spans ``min(start_ns, end_ns - window_s)`` through
        ``end_ns``. The first and last selected samples must be no farther
        than ``max_gap_s`` from those endpoints, and every adjacent selected
        sample must meet that same gap limit.
        """
        interval = self._interval(start_ns, end_ns)
        if interval is None:
            return self._summary(ready=False, still=False, reason="invalid_image_interval",
                                 requested_start_ns=None, requested_end_ns=None,
                                 window_start_ns=None, window_end_ns=None)
        window_start, window_end = interval
        gap_ns = int(round(self.settings["max_gap_s"] * 1_000_000_000))
        all_samples = list(self._samples)
        selected = [sample for sample in all_samples if window_start <= sample[0] <= window_end]

        base = dict(requested_start_ns=int(start_ns), requested_end_ns=int(end_ns),
                    window_start_ns=window_start, window_end_ns=window_end)
        if not all_samples:
            return self._summary(ready=False, still=False, reason="no_imu_samples", samples=selected, **base)
        if not selected:
            if all_samples[-1][0] < window_start - gap_ns:
                reason = "imu_too_old"
            elif all_samples[0][0] > window_end + gap_ns:
                reason = "imu_too_future"
            else:
                reason = "no_imu_coverage"
            return self._summary(ready=False, still=False, reason=reason, samples=selected, **base)
        if selected[0][0] - window_start > gap_ns:
            return self._summary(ready=False, still=False, reason="missing_start_coverage",
                                 samples=selected, **base)
        if window_end - selected[-1][0] > gap_ns:
            return self._summary(ready=False, still=False, reason="missing_end_coverage",
                                 samples=selected, **base)
        if any(second[0] - first[0] > gap_ns for first, second in zip(selected, selected[1:])):
            return self._summary(ready=False, still=False, reason="imu_gap_exceeds_limit",
                                 samples=selected, **base)
        if len(selected) < self.settings["min_samples"]:
            return self._summary(ready=False, still=False, reason="too_few_samples",
                                 samples=selected, **base)

        summary = self._summary(ready=False, still=False, reason="not_assessed", samples=selected, **base)
        motion_reasons: list[str] = []
        if summary["max_accel_norm_error_m_s2"] > self.settings["max_accel_norm_error_m_s2"]:
            motion_reasons.append("accel_norm_error")
        if summary["max_accel_axis_std_m_s2"] > self.settings["max_accel_std_m_s2"]:
            motion_reasons.append("accel_axis_std")
        # Norm includes a constant gyro bias, so this is deliberately stricter
        # than considering only gyro variance.
        if summary["max_gyro_norm_rad_s"] > self.settings["max_gyro_norm_rad_s"]:
            motion_reasons.append("gyro_motion")
        summary["still"] = not motion_reasons
        summary["ready"] = not motion_reasons
        summary["reason"] = "ready" if not motion_reasons else motion_reasons[0]
        return summary

    def export(self, start_ns: int, end_ns: int) -> dict[str, Any]:
        """Return the assessed raw window in a YAML/JSON-serializable document."""
        summary = self.assess(start_ns, end_ns)
        window_start, window_end = summary["window_start_ns"], summary["window_end_ns"]
        samples: list[dict[str, Any]] = []
        if window_start is not None and window_end is not None:
            for stamp, acceleration, angular_velocity in self._samples:
                if window_start <= stamp <= window_end:
                    samples.append({
                        "stamp_ns": stamp,
                        "frame_id": self.settings["frame_id"],
                        "linear_acceleration_m_s2": acceleration.tolist(),
                        "angular_velocity_rad_s": angular_velocity.tolist(),
                    })
        return {
            "schema_version": 1,
            "source": "synchronized_imu",
            "orientation_estimated": False,
            "configuration": dict(self.settings),
            "assessment": summary,
            "samples": samples,
        }
