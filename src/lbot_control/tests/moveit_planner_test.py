"""Exercise the actual CAD/KDL/OMPL/TOTG pipeline, without any robot clients."""
import copy
import csv
import importlib.util
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import unittest
import yaml
import numpy as np
from moveit_test_geometry import Geometry

EXECUTABLE = sys.argv.pop(1)
PACKAGE = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("moveit_config", PACKAGE / "python/moveit_config.py")
MODEL = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(MODEL)
TASK = yaml.safe_load((PACKAGE.parents[1] / "config/control/nut_task.yaml").read_text())["/**"]["ros__parameters"]
WP1 = [2.648, 2.432, -2.715, -1.078, .946, .039, .210]
WP2 = [2.681, 3.158, -2.315, -1.074, 1.293, -.427, .174]
FK1 = [.42234940712971325, .4204336706939131, -.22719896684576435,
       1.6398704413558565, .19140613168278206, -1.5270675260312314]
FK2 = [.32834155993529524, -.022261356560178296, -.29747956270668224,
       1.58515865252804, -.0194000230397028, -2.723737994155044]


class MoveItPlannerTest(unittest.TestCase):
    def run_planner(self, start, goal=None, extra=None):
        config = MODEL.parameters(TASK)
        config.update(preview_start_joints=start, preview_fk_only=goal is None,
                      preview_timeout_s=3., moveit_joint_velocity_rad_s=.12,
                      moveit_joint_acceleration_rad_s2=.12)
        if goal is not None:
            config["preview_goal_pose"] = goal
        config.update(extra or {})
        with tempfile.TemporaryDirectory(prefix="lbot_moveit_test_") as directory:
            path = Path(directory)
            config["preview_output"] = str(path / "trajectory.csv")
            (path / "params.yaml").write_text(yaml.safe_dump({"/**": {"ros__parameters": config}}, allow_unicode=True))
            env = dict(os.environ, ROS_DOMAIN_ID="181", RMW_IMPLEMENTATION="rmw_cyclonedds_cpp")
            result = subprocess.run([EXECUTABLE, "--ros-args", "--params-file", str(path / "params.yaml")],
                                    capture_output=True, text=True, timeout=25, env=env)
            output = result.stdout + result.stderr
            rows = []
            if (path / "trajectory.csv").exists():
                rows = [[float(v) for v in row] for row in list(csv.reader((path / "trajectory.csv").open()))[1:]]
            return result.returncode, output, rows

    def test_controller_joint_sign_and_frame_agree_with_measured_sdk_fk(self):
        for start, expected in ((WP1, FK1), (WP2, FK2)):
            code, text, rows = self.run_planner(start)
            self.assertEqual(code, 0, text)
            actual = [float(v) for v in re.search(r"MODEL_FK \[([^]]+)\]", text).group(1).split(",")]
            for a, b in zip(actual, expected):
                self.assertAlmostEqual(a, b, delta=1e-6)
            self.assertFalse(rows)

    def test_real_planner_produces_bounded_timed_joint_path(self):
        goal = list(FK2)
        goal[0] += .01
        code, text, rows = self.run_planner(WP2, goal)
        self.assertEqual(code, 0, text)
        self.assertGreater(len(rows), 10)
        self.assertEqual(rows[0][0], 0.)
        for a, b in zip(rows[0][1:], WP2):
            self.assertAlmostEqual(a, b, delta=1e-7)
        for row in rows:
            for q, lo, hi in zip(row[1:], TASK['left_joint_min'], TASK['left_joint_max']):
                self.assertTrue(lo <= q <= hi)
        for a, b in zip(rows, rows[1:]):
            self.assertGreater(b[0], a[0])
            self.assertLessEqual(max(abs(q-p)/(b[0]-a[0]) for p, q in zip(a[1:], b[1:])), .1213)
        # The sampled final FK must reach the requested Arm_Tip, not the palm.
        code, text, _ = self.run_planner(rows[-1][1:])
        self.assertEqual(code, 0, text)
        actual = [float(v) for v in re.search(r"MODEL_FK \[([^]]+)\]", text).group(1).split(",")]
        for a, b in zip(actual, goal):
            self.assertAlmostEqual(a, b, delta=.0005)

    def test_colliding_start_fails_without_trajectory(self):
        code, text, rows = self.run_planner(WP2, FK2, {"moveit_obstacle_boxes": [0., 0., 0., 3., 3., 3.]})
        self.assertNotEqual(code, 0, text)
        self.assertIn("start state rejected: collision", text)
        self.assertFalse(rows)

    def test_height_guard_covers_fixed_waypoint_and_retimed_interpolation(self):
        geometry = Geometry(MODEL.parameters(TASK)['robot_description'], TASK['moveit_hand_envelope'])
        start = [-.195, .106, -.146, -1.517, 1.426, -.200, -.089]
        end = [-.286, .032, .772, -1.501, .947, -.672, -.515]
        goal = geometry.pose(end)
        code, text, rows = self.run_planner(start, goal, {
            'moveit_transfer_keep_above': True, 'moveit_transfer_max_drop_m': .005,
            'preview_goal_joints': end, 'preview_timeout_s': 10.})
        self.assertEqual(code, 0, text)
        self.assertIn('MoveIt transfer height check:', text)
        floors = np.minimum(geometry.heights(start), geometry.heights(end))-.005
        # Validate independent FK between streamed samples, not just their endpoints.
        for a, b in zip(rows, rows[1:]):
            for fraction in (0., .5, 1.):
                joints = (1-fraction)*np.array(a[1:])+fraction*np.array(b[1:])
                self.assertTrue(np.all(geometry.heights(joints) >= floors-1e-6), (floors, joints))
        np.testing.assert_allclose(rows[-1][1:], end, atol=1e-5)

    def test_preserved_height_floor_rejects_lower_start_instead_of_relaxing(self):
        code, text, rows = self.run_planner(WP2, FK2, {
            'moveit_transfer_keep_above': True, 'preview_height_floors': [FK2[2]+.01, -1.]})
        self.assertNotEqual(code, 0, text)
        self.assertIn('start state rejected: transfer height floor violated', text)
        self.assertFalse(rows)

    def test_same_recorded_goal_prefers_ik_clear_of_joint_limit(self):
        # This accepted field target previously ended 0.9 microradians above
        # J2's lower limit, making measured settling error trip the follow gate.
        start = [-.2864122, -.0321970, .7719536, -1.5010300, .9468222, -.6673918, -.5170898]
        goal = [.337295, -.081038, -.259725, 1.289376, -.227438, -2.543683]
        code, text, rows = self.run_planner(start, goal, {'moveit_goal_preferred_clearance_rad': .005})
        self.assertEqual(code, 0, text)
        clearance = min(min(q-lo, hi-q) for q, lo, hi in zip(
            rows[-1][1:], TASK['left_joint_min'], TASK['left_joint_max']))
        self.assertGreaterEqual(clearance, .005-1e-7, text)
        code, text, _ = self.run_planner(rows[-1][1:])
        self.assertEqual(code, 0, text)
        actual = [float(v) for v in re.search(r'MODEL_FK \[([^]]+)\]', text).group(1).split(',')]
        for a, b in zip(actual, goal):
            self.assertAlmostEqual(a, b, delta=.0005)

    def test_out_of_bounds_start_and_unreachable_goal_fail(self):
        bad = list(WP2); bad[1] = 3.3
        code, text, rows = self.run_planner(bad, FK2)
        self.assertNotEqual(code, 0, text)
        self.assertIn("joint bounds violated", text)
        self.assertFalse(rows)
        code, text, rows = self.run_planner(WP2, [2., 2., 2., *FK2[3:]])
        self.assertNotEqual(code, 0, text)
        self.assertIn("no trajectory sent", text)
        self.assertFalse(rows)


if __name__ == "__main__":
    unittest.main()
