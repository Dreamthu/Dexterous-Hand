from __future__ import annotations

import unittest

import numpy as np

from linkerbot.extrinsic_calibration import saved_transform
from linkerbot.runtime import REPOSITORY_ROOT, load_yaml


RESULT = "artifacts/calibration/extrinsics/20260910_eye_to_hand_v2/extrinsics.yaml"


class CalibrationResultTests(unittest.TestCase):
    def test_integrated_result_is_loadable_and_rigid(self):
        document = load_yaml(RESULT)
        transform = saved_transform(document)

        self.assertEqual(document["transform"]["parent_frame"], "base_link")
        self.assertEqual(document["transform"]["child_frame"], "camera_color_optical_frame")
        self.assertEqual(document["camera_root_frame"], "camera_link")
        self.assertEqual(document["configuration"]["frames"]["base"], "base_link")
        self.assertTrue(document["quality"]["accepted"])

        rotation = transform[:3, :3]
        np.testing.assert_allclose(rotation @ rotation.T, np.eye(3), atol=1e-12)
        self.assertAlmostEqual(np.linalg.det(rotation), 1.0, places=12)
        np.testing.assert_allclose(
            transform[:3, 3],
            [0.09842810283401945, 0.05740493511149311, 0.24110711157764436],
        )

    def test_camera_info_matches_recorded_color_profile(self):
        camera_info = load_yaml(RESULT)["camera_info"]
        self.assertEqual(camera_info["image_width"], 1280)
        self.assertEqual(camera_info["image_height"], 720)
        self.assertEqual(camera_info["distortion_model"], "plumb_bob")
        self.assertEqual(len(camera_info["camera_matrix"]["data"]), 9)
        self.assertEqual(len(camera_info["distortion_coefficients"]["data"]), 8)

    def test_result_path_is_inside_calibration_artifact_tree(self):
        self.assertTrue((REPOSITORY_ROOT / RESULT).is_file())


if __name__ == "__main__":
    unittest.main()
