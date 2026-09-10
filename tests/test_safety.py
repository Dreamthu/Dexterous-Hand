from __future__ import annotations

import copy
import io
from contextlib import redirect_stdout
from concurrent.futures import Future
from pathlib import Path
from tempfile import TemporaryDirectory
from types import SimpleNamespace
import unittest
from unittest.mock import Mock, patch

import cv2
import numpy as np

from extrinsic_calibration.cli import (
    DEFAULT_CONFIG, _same_capture_contract, new_dataset, publish_command,
    solve_command, verify_command,
)
from extrinsic_calibration.core import (
    CalibrationError, atomic_write_json, atomic_write_yaml, invert_transform,
    load_config, load_yaml, pose_dict, transform_from_dict,
)
from extrinsic_calibration.safety import (
    base_from_camera_root, stationary_window, validate_camera_tf_tree, validate_tool_snapshot,
)
from extrinsic_calibration.tool_monitor import ToolMonitor


def transform(rvec, translation):
    result = np.eye(4)
    result[:3, :3] = cv2.Rodrigues(np.asarray(rvec, dtype=float))[0]
    result[:3, 3] = translation
    return result


class CaptureSafetyTests(unittest.TestCase):
    def setUp(self):
        self.config = load_config(DEFAULT_CONFIG)
        self.capture = self.config["capture"]
        self.poses = [(float(t), "base_link", np.eye(4)) for t in np.linspace(9.5, 10.0, 26)]

    def check(self, poses):
        return stationary_window(poses, 10.0, "base_link", self.capture)

    def test_full_stationary_window_passes(self):
        self.assertTrue(self.check(self.poses).passed)

    def test_three_recent_messages_do_not_prove_stationarity(self):
        self.assertFalse(self.check(self.poses[-3:]).passed)

    def test_gap_duplicate_backwards_stale_and_wrong_frame_fail(self):
        bad_inputs = [self.poses[:5] + self.poses[18:],
                      self.poses[:15] + [self.poses[14]] + self.poses[15:],
                      self.poses[:15] + self.poses[14:], self.poses[:-7],
                      [(t, "base_torso_root", m) for t, _, m in self.poses]]
        for poses in bad_inputs:
            with self.subTest(poses=poses):
                self.assertFalse(self.check(poses).passed)

    def test_moving_robot_fails(self):
        self.poses[-1][2][0, 3] = 0.01
        self.assertFalse(self.check(self.poses).passed)

    def test_motion_in_nearest_pose_after_image_is_also_checked(self):
        moved = np.eye(4)
        moved[0, 3] = 0.01
        self.poses.append((10.02, "base_link", moved))
        self.assertTrue(self.check(self.poses).passed)
        result = stationary_window(self.poses, 10.0, "base_link", self.capture, 10.02)
        self.assertFalse(result.passed)

    def test_tool_name_offset_and_finite_checks(self):
        snapshot = {"name": "Arm_Tip", "translation_m": [0, 0, 0], "euler_rad": [0, 0, 0]}
        validate_tool_snapshot(snapshot, self.config["robot"])
        for key, value in (("name", "Other"), ("translation_m", [0, 0, 0.001]),
                           ("euler_rad", [0, 0, 0.1]), ("euler_rad", [0, 0, float("nan")])):
            with self.subTest(key=key, value=value), self.assertRaises(CalibrationError):
                validate_tool_snapshot(dict(snapshot, **{key: value}), self.config["robot"])

    def test_legacy_contract_cannot_be_appended_as_new_data(self):
        dataset = new_dataset(self.config, "calibration", {})
        _same_capture_contract(dataset, self.config, "calibration")
        del dataset["robot_contract"]
        with self.assertRaisesRegex(CalibrationError, "tool contract"):
            _same_capture_contract(dataset, self.config, "calibration")

    def test_legacy_config_without_tf_contract_cannot_publish(self):
        # Historical data/configuration are archived, not test fixtures or runtime dependencies.
        legacy = copy.deepcopy(self.config)
        legacy.pop("robot")
        legacy.pop("tf_publish")
        legacy["frames"]["robot_pose_child_frame"] = "arm_right_R8_Link"
        with self.assertRaises(CalibrationError):
            publish_command(legacy, True)

    def test_new_config_validation(self):
        for section, key, value in (("capture", "maximum_pose_gap_s", 0.5),
                                    ("robot", "expected_tool_translation_m", [0, 0]),
                                    ("robot", "expected_tool_name", "Wrong"),
                                    ("tf_publish", "camera_root_frame", "camera_color_optical_frame"),
                                    ("tf_publish", "lookup_timeout_s", 0.5)):
            config = copy.deepcopy(self.config)
            config[section][key] = value
            with TemporaryDirectory() as directory:
                path = Path(directory) / "config.yaml"
                atomic_write_yaml(path, config)
                with self.subTest(key=key), self.assertRaises(CalibrationError):
                    load_config(path)


class TfSafetyTests(unittest.TestCase):
    def setUp(self):
        self.parents = {"optical": {"color"}, "color": {"root"}}
        self.edges = {("color", "optical"), ("root", "color")}

    def test_internal_static_tree_passes(self):
        validate_camera_tf_tree(self.parents, self.edges, "root", "optical", "base")

    def test_conflicting_optical_parent_and_attached_root_fail(self):
        for parents in (dict(self.parents, optical={"base", "color"}),
                        dict(self.parents, root={"base"}), dict(self.parents, color=set()),
                        dict(self.parents, color={"optical"})):
            with self.subTest(parents=parents), self.assertRaises(CalibrationError):
                validate_camera_tf_tree(parents, self.edges, "root", "optical", "base")

    def test_dynamic_internal_path_rejected(self):
        with self.assertRaises(CalibrationError):
            validate_camera_tf_tree(self.parents, {("root", "color")}, "root", "optical", "base")

    def test_base_cannot_already_be_below_camera_root(self):
        with self.assertRaises(CalibrationError):
            validate_camera_tf_tree(dict(self.parents, base={"root"}), self.edges,
                                    "root", "optical", "base")

    def test_camera_root_composition_preserves_optical_solution(self):
        base_optical = transform([2.4, 0.1, -0.3], [0.1, 0.2, 0.3])
        root_optical = transform([-1.1, 0.7, -1.2], [0.02, -0.01, 0.005])
        base_root = base_from_camera_root(base_optical, root_optical)
        np.testing.assert_allclose(base_root @ root_optical, base_optical, atol=1e-12)
        self.assertFalse(np.allclose(base_root, base_optical))


class ToolMonitorTests(unittest.TestCase):
    def setUp(self):
        config = load_config(DEFAULT_CONFIG)["robot"]
        self.client = Mock()
        self.client.service_is_ready.return_value = True
        self.future = Future()
        self.client.call_async.return_value = self.future
        node = Mock()
        node.create_client.return_value = self.client
        with patch("extrinsic_calibration.tool_monitor.tool_service_class", return_value=SimpleNamespace(Request=lambda: None)):
            self.monitor = ToolMonitor(node, config)

    def reply(self, name="Arm_Tip"):
        vec = SimpleNamespace(x=0.0, y=0.0, z=0.0)
        return SimpleNamespace(success=True, frame=SimpleNamespace(name=name, position=vec, euler=vec))

    def test_checks_then_rejects_stale_observation(self):
        with patch("extrinsic_calibration.tool_monitor.time.monotonic", return_value=10.0):
            self.monitor.poll()
            self.future.set_result(self.reply())
            self.monitor.poll()
            self.assertEqual(self.monitor.require_current()["name"], "Arm_Tip")
        with patch("extrinsic_calibration.tool_monitor.time.monotonic", return_value=12.0):
            with self.assertRaisesRegex(CalibrationError, "stale"):
                self.monitor.require_current()

    def test_changed_tool_latches_failure(self):
        self.monitor.poll()
        self.future.set_result(self.reply("Other"))
        self.monitor.poll()
        with self.assertRaisesRegex(CalibrationError, "end this session"):
            self.monitor.require_current()
        self.monitor.poll()
        self.assertTrue(self.monitor.latched_error)

    def test_unavailable_and_timeout_block_capture(self):
        self.client.service_is_ready.return_value = False
        self.monitor.poll()
        with self.assertRaisesRegex(CalibrationError, "unavailable"):
            self.monitor.require_current()
        self.client.service_is_ready.return_value = True
        with patch("extrinsic_calibration.tool_monitor.time.monotonic", return_value=10.0):
            self.monitor.poll()
        self.client.service_is_ready.return_value = False
        with patch("extrinsic_calibration.tool_monitor.time.monotonic", return_value=13.0):
            self.monitor.poll()
        self.assertTrue(self.future.cancelled())
        with self.assertRaises(CalibrationError):
            self.monitor.require_current()


class WorkflowSafetyTests(unittest.TestCase):
    def setUp(self):
        self.temporary = TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        root = Path(self.temporary.name)
        self.config = load_config(DEFAULT_CONFIG)
        self.config["capture"]["calibration_dataset"] = str(root / "training.json")
        self.config["capture"]["validation_dataset"] = str(root / "validation.json")
        self.config["solve"]["result"] = str(root / "result.yaml")
        self.base_optical = transform([2.6, 0.1, -0.2], [0.1, 0.05, 0.3])
        mounting = transform([0.2, 0.1, 0.0], [0.02, 0.01, 0.08])
        random = np.random.default_rng(51)
        samples = []
        for index in range(32):
            robot = transform(random.normal(size=3), random.uniform(-0.3, 0.3, size=3))
            board = invert_transform(self.base_optical) @ robot @ mounting
            samples.append({"image_stamp": str(index), "base_from_gripper": pose_dict(robot),
                            "camera_from_board": pose_dict(board), "reprojection_rms_px": 0.2,
                            "tool_snapshot": {"name": "Arm_Tip", "translation_m": [0, 0, 0],
                                              "euler_rad": [0, 0, 0]}})
        self.training = new_dataset(self.config, "calibration", {})
        self.training["samples"] = samples[:24]
        self.validation = new_dataset(self.config, "validation", {})
        self.validation["samples"] = samples[24:]
        self.save_datasets()

    def save_datasets(self):
        atomic_write_json(Path(self.config["capture"]["calibration_dataset"]), self.training)
        atomic_write_json(Path(self.config["capture"]["validation_dataset"]), self.validation)

    def solve(self):
        with redirect_stdout(io.StringIO()):
            self.assertEqual(solve_command(self.config), 0)

    def test_solve_verify_publish_dry_run_uses_camera_root(self):
        self.solve()
        with redirect_stdout(io.StringIO()):
            self.assertEqual(verify_command(self.config), 0)
        result = load_yaml(Path(self.config["solve"]["result"]))
        self.assertTrue(result["quality"]["accepted"])
        self.assertEqual(len(result["sample_residuals"]), 24)
        np.testing.assert_allclose(transform_from_dict(result), self.base_optical, atol=1e-8)
        internal = transform([-1.0, 0.5, -1.0], [0.02, 0.01, 0.0])
        output = io.StringIO()
        with patch("extrinsic_calibration.tf_publish.lookup_camera_internal_transform", return_value=internal), \
                patch("extrinsic_calibration.cli.os.execvp") as execute, redirect_stdout(output):
            self.assertEqual(publish_command(self.config, True), 0)
            execute.assert_not_called()
        self.assertIn("--frame-id base_link --child-frame-id camera_link", output.getvalue())

    def test_publish_refuses_unvalidated_result_before_tf_lookup(self):
        self.solve()
        with patch("extrinsic_calibration.tf_publish.lookup_camera_internal_transform") as lookup:
            with self.assertRaisesRegex(CalibrationError, "not passed"):
                publish_command(self.config, True)
            lookup.assert_not_called()

    def test_copied_training_data_rejected_even_after_changing_stamp(self):
        self.solve()
        self.validation["samples"][0] = copy.deepcopy(self.training["samples"][0])
        self.validation["samples"][0]["image_stamp"] = "new-label"
        self.save_datasets()
        with self.assertRaisesRegex(CalibrationError, "reuses training"):
            verify_command(self.config)

    def test_verify_replaces_old_failure_messages(self):
        self.solve()
        path = Path(self.config["solve"]["result"])
        result = load_yaml(path)
        result["quality"]["failures"] = ["old validation failure"]
        atomic_write_yaml(path, result)
        with redirect_stdout(io.StringIO()):
            self.assertEqual(verify_command(self.config), 0)
        self.assertEqual(load_yaml(path)["quality"]["failures"], [])


if __name__ == "__main__":
    unittest.main()
