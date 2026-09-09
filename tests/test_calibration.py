from __future__ import annotations

import unittest

import cv2
import numpy as np

from extrinsic_calibration.core import (
    invert_transform,
    load_config,
    matrix_from_pose,
    pose_dict,
    rotation_error_deg,
    solve_eye_to_hand,
    validation_metrics,
)
from extrinsic_calibration.cli import DEFAULT_CONFIG


def transform_from_rvec(rvec, translation):
    transform = np.eye(4)
    transform[:3, :3] = cv2.Rodrigues(np.asarray(rvec, dtype=float))[0]
    transform[:3, 3] = np.asarray(translation, dtype=float)
    return transform


class ExtrinsicCalibrationTests(unittest.TestCase):
    def test_right_wrist_configuration_matches_fixed_camera_setup(self) -> None:
        config = load_config(DEFAULT_CONFIG)
        self.assertEqual(config["calibration_type"], "eye_to_hand")
        self.assertEqual(config["topics"]["robot_pose"], "/robot1/right_arm/pose_states")
        self.assertEqual(config["frames"]["base_frame"], "base_torso_root")
        self.assertEqual(config["frames"]["camera_frame"], "camera_color_optical_frame")
        self.assertEqual(config["board"]["dictionary"], "DICT_6X6_250")
        self.assertEqual(config["board"]["markers_x"], 3)
        self.assertEqual(config["board"]["markers_y"], 3)

    def test_eye_to_hand_solver_returns_base_from_camera_not_inverse(self) -> None:
        random = np.random.default_rng(42)
        expected_base_from_camera = transform_from_rvec(
            [2.75, 0.22, -0.16], [0.31, -0.08, 0.91]
        )
        expected_gripper_from_board = transform_from_rvec(
            [0.21, -0.13, 0.08], [0.035, -0.012, 0.105]
        )
        samples = []
        for index in range(24):
            axis = random.normal(size=3)
            axis /= np.linalg.norm(axis)
            base_from_gripper = transform_from_rvec(
                axis * random.uniform(0.15, 2.2),
                [
                    random.uniform(0.12, 0.58),
                    random.uniform(-0.42, 0.38),
                    random.uniform(0.05, 0.62),
                ],
            )
            camera_from_board = (
                invert_transform(expected_base_from_camera)
                @ base_from_gripper
                @ expected_gripper_from_board
            )
            samples.append(
                {
                    "base_from_gripper": pose_dict(base_from_gripper),
                    "camera_from_board": pose_dict(camera_from_board),
                    "reprojection_rms_px": 0.2 + index * 0.001,
                }
            )

        solve_config = {
            "method": "PARK",
            "minimum_samples": 15,
            "outlier_translation_m": 0.015,
            "outlier_rotation_deg": 2.0,
            "maximum_outlier_iterations": 3,
        }
        solved = solve_eye_to_hand(samples, solve_config)
        translation_error = np.linalg.norm(
            solved.base_from_camera[:3, 3] - expected_base_from_camera[:3, 3]
        )
        self.assertLess(translation_error, 1e-8)
        self.assertLess(rotation_error_deg(solved.base_from_camera, expected_base_from_camera), 1e-6)
        self.assertGreater(
            np.linalg.norm(
                solved.base_from_camera[:3, 3]
                - invert_transform(expected_base_from_camera)[:3, 3]
            ),
            0.1,
        )

        metrics = validation_metrics(
            samples[::4], solved.base_from_camera, solved.gripper_from_board
        )
        self.assertLess(metrics["validation_translation_rms_m"], 1e-8)
        self.assertLess(metrics["validation_rotation_rms_deg"], 1e-6)

    def test_pose_round_trip(self) -> None:
        expected = matrix_from_pose([0.1, -0.2, 0.3], [0.2, -0.3, 0.1, 0.9])
        actual = matrix_from_pose(**{
            "translation": pose_dict(expected)["translation_m"],
            "quaternion_xyzw": pose_dict(expected)["rotation_xyzw"],
        })
        np.testing.assert_allclose(actual, expected, atol=1e-12)


if __name__ == "__main__":
    unittest.main()
