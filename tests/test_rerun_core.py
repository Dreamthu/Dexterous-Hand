import math
from pathlib import Path
import sys
import unittest

import numpy as np


REPOSITORY_ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(REPOSITORY_ROOT / "src/lbot_rerun"))

from lbot_rerun.core import (  # noqa: E402
    JointSpec,
    axis_angle_quaternion,
    downsample_points,
    interpolate_hand_command,
    joint_transform,
    parse_left_urdf,
    rotate_vector,
)


class RerunCoreTest(unittest.TestCase):
    def test_left_model_uses_controller_base_as_root(self):
        model = parse_left_urdf(
            REPOSITORY_ROOT / "assets/workstations/lkls73_i1_o6_bimanual/workstation.urdf"
        )
        self.assertEqual(model.root_link, "base_base_link")
        self.assertEqual(model.link_entity_name(model.root_link), "base_link")
        self.assertEqual(len(model.arm_joint_names), 7)
        self.assertIn("hand_left_lh_hand_base_link", model.links)
        self.assertNotIn("arm_right_R1_Link", model.links)
        self.assertEqual(
            model.link_path("world/base_link/robot/left", "arm_left_L3_Link"),
            "world/base_link/robot/left/base_arm_left_mount/arm_left_L_arm_root/arm_left_L1_Link/arm_left_L2_Link/arm_left_L3_Link",
        )

    def test_revolute_joint_composes_origin_and_axis_rotation(self):
        joint = JointSpec(
            name="test",
            joint_type="revolute",
            parent_link="parent",
            child_link="child",
            origin_xyz=(0.1, 0.2, 0.3),
            axis=(0.0, 0.0, 1.0),
        )
        translation, quaternion = joint_transform(joint, math.pi / 2)
        self.assertEqual(translation, (0.1, 0.2, 0.3))
        rotated = rotate_vector(quaternion, (1.0, 0.0, 0.0))
        np.testing.assert_allclose(rotated, (0.0, 1.0, 0.0), atol=1e-6)

    def test_o6_proxy_mapping_honors_per_finger_direction(self):
        result = interpolate_hand_command(
            [0, 255, 0, 0, 0, 0],
            [255, 40, 255, 255, 255, 255],
            [0, 40, 0, 0, 0, 0],
            ((0.0, 1.3), (0.0, 0.58), (0.0, 1.6), (0.0, 1.6), (0.0, 1.6), (0.0, 1.6)),
        )
        self.assertAlmostEqual(result[0], 1.3)
        self.assertAlmostEqual(result[1], 0.0)
        self.assertAlmostEqual(result[2], 1.6)

    def test_downsample_removes_invalid_points_and_bounds_output(self):
        points = np.array(
            [[0.0, 0.0, 0.0], [np.nan, 1.0, 2.0], [1.0, 1.0, 1.0], [2.0, 2.0, 2.0]],
            dtype=np.float32,
        )
        result = downsample_points(points, 2)
        self.assertEqual(result.shape, (2, 3))
        self.assertTrue(np.isfinite(result).all())


if __name__ == "__main__":
    unittest.main()
