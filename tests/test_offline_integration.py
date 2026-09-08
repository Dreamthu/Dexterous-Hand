"""Merge regression contracts; these do not replace ROS/hardware integration tests."""
from __future__ import annotations

from pathlib import Path
import subprocess
import sys
import tempfile
from types import SimpleNamespace
import unittest
from unittest.mock import Mock, patch

from linkerbot import runtime
from linkerbot.runtime import ConfigurationError, REPOSITORY_ROOT, detector_command


class OfflineRuntimeIntegrationTests(unittest.TestCase):
    def test_offline_import_and_configuration_work_without_fcntl(self):
        # A fresh interpreter catches transitive imports on both Windows and Linux.
        program = '''
import builtins
original_import = builtins.__import__
def without_fcntl(name, *args, **kwargs):
    if name == "fcntl":
        raise ModuleNotFoundError("No module named fcntl", name="fcntl")
    return original_import(name, *args, **kwargs)
builtins.__import__ = without_fcntl
from linkerbot.offline import detector_parameters, settings
from linkerbot.runtime import detector_command, extrinsic_calibration_command
config = settings("config/vision/offline.yaml")
assert detector_parameters(config["detector_config"])
assert detector_command(require_built=False)
assert extrinsic_calibration_command(["--help"])
'''
        result = subprocess.run([sys.executable, "-c", program], cwd=REPOSITORY_ROOT,
                                capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stdout + result.stderr)

    def test_offline_and_camera_help_work_from_another_directory(self):
        with tempfile.TemporaryDirectory() as directory:
            for script in ("offline_detection.py", "run_camera.py", "run_perception.py"):
                with self.subTest(script=script):
                    result = subprocess.run(
                        [sys.executable, str(REPOSITORY_ROOT / "apps" / script), "--help"],
                        cwd=directory, capture_output=True, text=True)
                    self.assertEqual(result.returncode, 0, result.stderr)
                    self.assertIn("usage:", result.stdout)

    def test_camera_dry_run_needs_neither_ros_nor_a_camera_lock(self):
        result = subprocess.run(
            [sys.executable, str(REPOSITORY_ROOT / "apps/run_camera.py"), "--dry-run"],
            cwd=REPOSITORY_ROOT, capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, result.stderr)
        self.assertIn("ros2 launch orbbec_camera gemini2.launch.py", result.stdout)
        self.assertIn("enable_sync_output_accel_gyro:=true", result.stdout)

    def test_detector_paths_are_platform_native(self):
        command = detector_command(require_built=False)
        self.assertEqual(Path(command[0]), REPOSITORY_ROOT / "install/lbot_vision/lib/lbot_vision/nut_detector_node")
        self.assertEqual(Path(command[3]), REPOSITORY_ROOT / "config/vision/nut_detector.yaml")

    def test_non_posix_live_camera_fails_with_actionable_error(self):
        with patch.object(runtime, "os", SimpleNamespace(name="nt")):
            with self.assertRaisesRegex(ConfigurationError, "require Linux/POSIX"):
                runtime.acquire_camera_lock()

    def test_posix_without_fcntl_fails_with_actionable_error(self):
        with patch.object(runtime, "os", SimpleNamespace(name="posix")), \
                patch.dict(sys.modules, {"fcntl": None}):
            with self.assertRaisesRegex(ConfigurationError, "requires Python fcntl"):
                runtime.acquire_camera_lock()

    def test_lock_file_is_closed_on_contention_and_io_failure(self):
        # Simulated POSIX adapter tests resource cleanup, not real OS flock behavior.
        fake_os = SimpleNamespace(name="posix", environ={}, getuid=lambda: 123,
                                  getpid=lambda: 456)
        for failure in (BlockingIOError(), OSError("lock unavailable")):
            with self.subTest(failure=type(failure).__name__):
                lock_file = Mock()
                lock_file.read.return_value = "456"
                fcntl = SimpleNamespace(LOCK_EX=2, LOCK_NB=4, flock=Mock(side_effect=failure))
                with patch.object(runtime, "os", fake_os), \
                        patch.dict(sys.modules, {"fcntl": fcntl}), \
                        patch.object(Path, "open", return_value=lock_file):
                    with self.assertRaises(ConfigurationError):
                        runtime.acquire_camera_lock()
                lock_file.close.assert_called_once()
                lock_file.truncate.assert_not_called()

    def test_camera_lock_directory_errors_are_actionable(self):
        fake_os = SimpleNamespace(name="posix", environ={}, getuid=lambda: 123)
        with patch.object(runtime, "os", fake_os), \
                patch.dict(sys.modules, {"fcntl": SimpleNamespace()}), \
                patch.object(Path, "open", side_effect=PermissionError("read-only directory")):
            with self.assertRaisesRegex(ConfigurationError, "Check XDG_RUNTIME_DIR"):
                runtime.acquire_camera_lock()

    def test_perception_releases_lock_when_camera_exits_early(self):
        from apps import run_perception
        camera = Mock()
        camera.poll.return_value = 7
        camera.returncode = 7
        lock_file = Mock()
        with patch.object(sys, "argv", ["run_perception.py"]), \
                patch.object(run_perception, "camera_command", return_value=(["camera"], {})), \
                patch.object(run_perception, "detector_command", return_value=["detector"]), \
                patch.object(run_perception, "acquire_camera_lock", return_value=lock_file), \
                patch.object(run_perception.subprocess, "Popen", return_value=camera) as spawn, \
                patch.object(run_perception.time, "sleep"):
            self.assertEqual(run_perception.main(), 7)
        spawn.assert_called_once()
        lock_file.close.assert_called_once()


class MergeSourceContracts(unittest.TestCase):
    def test_color_only_diagnostics_preserve_sequence_and_freshness_guards(self):
        # Structural guard only: ROS callbacks/messages still require a Jazzy build.
        source = (REPOSITORY_ROOT / "src/lbot_vision/src/ros/nut_detector_node.cpp").read_text(encoding="utf-8")
        process = source.split("  void process()", 1)[1].split("  void publish_debug", 1)[0]
        markers = ["if (!color_is_fresh())", "stamp_ns == last_processed_ns_",
                   "lbot_vision::detect_2d", "sequence_->observe(", "publish_sequence_state();  // 2D",
                   "if (!depth_msg_ || !camera_geometry_)", "if (!sequence_snapshot.initialized",
                   "if (!observation.basket_found)", "cv::Mat depth;"]
        offsets = [process.index(marker) for marker in markers]
        self.assertEqual(offsets, sorted(offsets))
        waiting = process[offsets[5]:offsets[6]]
        for status in ("waiting_for_depth", "waiting_for_camera_info", "waiting_for_depth_and_camera_info"):
            self.assertIn('"' + status + '"', waiting)
        self.assertIn("cv::putText", waiting)
        self.assertIn("return publish_debug", waiting)
        self.assertNotIn("find_geometry(", source)
        self.assertEqual(source.count("lbot_vision::detect_2d("), 1)
        self.assertNotIn("<<<<<<<", source)

    def test_local_workspace_override_is_loaded_before_path_resolution(self):
        source = (REPOSITORY_ROOT / "scripts/_common.sh").read_text(encoding="utf-8")
        defaults = source.index('source "${LINKERBOT_ROOT}/config/workspace.env"')
        override = source.index('source "${LINKERBOT_ROOT}/config/workspace.local.env"')
        resolve = source.index('LINKERBOT_ROS_SETUP="$(linkerbot_resolve_path')
        self.assertLess(defaults, override)
        self.assertLess(override, resolve)


if __name__ == "__main__":
    unittest.main()
