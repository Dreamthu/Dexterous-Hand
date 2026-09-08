from __future__ import annotations

import unittest
import json

import numpy as np

from linkerbot.imu_sampling import ImuWindow


FRAME = "camera_accel_gyro_optical_frame"
BASE_SETTINGS = {
    "frame_id": FRAME,
    "window_s": 1.0,
    "min_samples": 50,
    "max_gap_s": 0.10,
    "gravity_m_s2": 9.80665,
    "max_accel_norm_error_m_s2": 0.6,
    "max_accel_std_m_s2": 0.15,
    "max_gyro_norm_rad_s": 0.035,
}


def populated_window(*, acceleration=(0.0, 0.0, -9.80665), gyro=(0.0, 0.0, 0.0),
                     start_ns=1_000_000_000, count=101, period_ns=10_000_000) -> ImuWindow:
    window = ImuWindow(BASE_SETTINGS)
    for index in range(count):
        assert window.add(start_ns + index * period_ns, FRAME, acceleration, gyro)
    return window


class ImuSamplingTests(unittest.TestCase):
    def test_valid_stationary_window_accepts_gravity_in_a_rotated_axis(self):
        # Gravity may point along any camera-fixed axis; only its SI norm is used.
        window = populated_window(acceleration=(9.80665, 0.0, 0.0), gyro=(0.002, -0.001, 0.001))
        result = window.assess(1_000_000_000, 2_000_000_000)

        self.assertTrue(result["ready"])
        self.assertTrue(result["still"])
        self.assertEqual(result["reason"], "ready")
        self.assertEqual(result["sample_count"], 101)
        self.assertAlmostEqual(result["mean_accel_norm_m_s2"], 9.80665)
        self.assertLess(result["max_gyro_norm_rad_s"], BASE_SETTINGS["max_gyro_norm_rad_s"])
        np.testing.assert_allclose(result["gyro_axis_std_rad_s"], [0.0, 0.0, 0.0], atol=1e-15)

    def test_motion_is_rejected_using_gyro_norm_and_acceleration(self):
        rotating = populated_window(gyro=(0.0, 0.0, 0.050))
        gyro_result = rotating.assess(1_000_000_000, 2_000_000_000)
        self.assertFalse(gyro_result["ready"])
        self.assertFalse(gyro_result["still"])
        self.assertEqual(gyro_result["reason"], "gyro_motion")

        accelerating = populated_window(acceleration=(0.0, 0.0, -10.55))
        accel_result = accelerating.assess(1_000_000_000, 2_000_000_000)
        self.assertEqual(accel_result["reason"], "accel_norm_error")
        self.assertGreater(accel_result["max_accel_norm_error_m_s2"], 0.6)

    def test_axis_motion_is_rejected_even_when_acceleration_norm_is_constant(self):
        window = ImuWindow(BASE_SETTINGS)
        for index in range(101):
            x = -0.3 if index % 2 else 0.3
            z = -np.sqrt(9.80665 ** 2 - x ** 2)
            window.add(1_000_000_000 + index * 10_000_000, FRAME, (x, 0, z), (0, 0, 0))
        result = window.assess(1_000_000_000, 2_000_000_000)
        self.assertLess(result["accel_norm_std_m_s2"], 1e-12)
        self.assertEqual(result["reason"], "accel_axis_std")

    def test_settings_reject_nonfinite_values_invalid_frames_and_impossible_buffer(self):
        for invalid in ({"frame_id": "/imu"}, {"max_gap_s": float("nan")},
                        {"window_s": "1"}, {"max_gyro_norm_rad_s": 0},
                        {"min_samples": 2.0}, {"max_samples": 49}, {"windwo_s": 1}):
            with self.subTest(invalid=invalid), self.assertRaises(ValueError):
                ImuWindow({**BASE_SETTINGS, **invalid})

    def test_missing_endpoint_and_internal_gap_reject_coverage(self):
        late_start = ImuWindow(BASE_SETTINGS)
        for index in range(91):
            late_start.add(1_110_000_000 + index * 10_000_000, FRAME, (0, 0, -9.80665), (0, 0, 0))
        self.assertEqual(late_start.assess(1_000_000_000, 2_000_000_000)["reason"], "missing_start_coverage")

        gapped = ImuWindow(BASE_SETTINGS)
        for index in range(101):
            if 40 <= index <= 55:
                continue
            gapped.add(1_000_000_000 + index * 10_000_000, FRAME, (0, 0, -9.80665), (0, 0, 0))
        self.assertEqual(gapped.assess(1_000_000_000, 2_000_000_000)["reason"], "imu_gap_exceeds_limit")

    def test_old_future_and_insufficient_samples_have_explicit_reasons(self):
        old = populated_window(start_ns=100_000_000, count=50)
        self.assertEqual(old.assess(1_000_000_000, 2_000_000_000)["reason"], "imu_too_old")

        future = populated_window(start_ns=3_000_000_000)
        self.assertEqual(future.assess(1_000_000_000, 2_000_000_000)["reason"], "imu_too_future")

        few = populated_window(count=49, period_ns=20_833_333)
        self.assertEqual(few.assess(1_000_000_000, 2_000_000_000)["reason"], "too_few_samples")

    def test_wrong_frame_nonfinite_duplicate_and_backward_stamp_are_not_joined(self):
        window = ImuWindow(BASE_SETTINGS)
        self.assertFalse(window.add(1_000_000_000, "another_imu", (0, 0, -9.80665), (0, 0, 0)))
        self.assertEqual(window.last_add_reason, "wrong_frame")
        self.assertFalse(window.add(1_000_000_000, FRAME, (np.nan, 0, 0), (0, 0, 0)))
        self.assertEqual(window.last_add_reason, "nonfinite_or_invalid_vector")
        self.assertFalse(window.add(0, FRAME, (0, 0, -9.80665), (0, 0, 0)))
        self.assertEqual(window.last_add_reason, "invalid_stamp")

        self.assertTrue(window.add(2_000_000_000, FRAME, (0, 0, -9.80665), (0, 0, 0)))
        self.assertFalse(window.add(2_000_000_000, FRAME, (0, 0, -9.80665), (0, 0, 0)))
        self.assertEqual(window.last_add_reason, "duplicate")
        self.assertTrue(window.add(1_000_000_000, FRAME, (0, 0, -9.80665), (0, 0, 0)))
        self.assertEqual(window.last_add_reason, "backward_stamp_reset")
        self.assertEqual(window.sample_count, 1)
        self.assertFalse(window.assess(1_000_000_000, 1_000_000_000)["ready"])

    def test_short_capture_requires_minimum_history_and_long_capture_is_not_truncated(self):
        short = populated_window(start_ns=1_500_000_000, count=51)
        short_result = short.assess(1_500_000_000, 2_000_000_000)
        self.assertEqual(short_result["window_start_ns"], 1_000_000_000)
        self.assertEqual(short_result["reason"], "missing_start_coverage")

        long = populated_window(count=201)
        long_result = long.assess(1_000_000_000, 3_000_000_000)
        self.assertTrue(long_result["ready"])
        self.assertEqual(long_result["sample_count"], 201)
        self.assertEqual(long_result["window_start_ns"], 1_000_000_000)

    def test_invalid_vectors_are_rejected_without_mutating_buffer(self):
        window = populated_window()
        for acceleration in ([[1], 0, 0], [1, 2], [1, 2, float("inf")],
                             ["1", "2", "3"], np.asarray(1), True):
            with self.subTest(acceleration=acceleration):
                self.assertFalse(window.add(2_010_000_000, FRAME, acceleration, (0, 0, 0)))
                self.assertEqual(window.sample_count, 101)
        self.assertTrue(window.add(2_010_000_000, FRAME, np.array([0, 0, -9.80665]), (0, 0, 0)))

    def test_buffer_is_bounded_and_clear_discards_previous_data(self):
        window = ImuWindow({**BASE_SETTINGS, "max_samples": 50})
        for index in range(101):
            window.add(1_000_000_000 + index * 10_000_000, FRAME, (0, 0, -9.80665), (0, 0, 0))
        self.assertEqual(window.sample_count, 50)
        self.assertEqual(window.assess(1_000_000_000, 2_000_000_000)["reason"], "missing_start_coverage")
        window.clear()
        self.assertEqual(window.assess(1_000_000_000, 2_000_000_000)["reason"], "no_imu_samples")

    def test_export_is_serializable_and_contains_only_selected_window_samples(self):
        window = populated_window(start_ns=500_000_000, count=201)
        document = window.export(1_000_000_000, 2_000_000_000)
        self.assertFalse(document["orientation_estimated"])
        self.assertEqual(document["configuration"]["frame_id"], FRAME)
        self.assertTrue(document["assessment"]["ready"])
        self.assertEqual(len(document["samples"]), 101)
        self.assertEqual(document["samples"][0]["stamp_ns"], 1_000_000_000)
        self.assertEqual(document["samples"][-1]["stamp_ns"], 2_000_000_000)
        self.assertEqual(json.loads(json.dumps(document, allow_nan=False)), document)


if __name__ == "__main__":
    unittest.main()
