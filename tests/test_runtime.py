from __future__ import annotations

import os
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch

from linkerbot.runtime import (
    ConfigurationError,
    REPOSITORY_ROOT,
    acquire_camera_lock,
    camera_command,
    detector_command,
    load_yaml,
    viewer_command,
)


class RuntimeConfigurationTests(unittest.TestCase):
    def test_camera_lock_rejects_a_second_entry_point(self) -> None:
        if os.name != "posix":
            with self.assertRaisesRegex(ConfigurationError, "require Linux/POSIX"):
                acquire_camera_lock()
            return  # Windows refusal is tested; POSIX contention requires Linux.
        with tempfile.TemporaryDirectory() as runtime_directory:
            with patch.dict(os.environ, {"XDG_RUNTIME_DIR": runtime_directory}):
                first_lock = acquire_camera_lock()
                self.addCleanup(first_lock.close)
                with self.assertRaisesRegex(
                    ConfigurationError, "camera application is already running"
                ):
                    acquire_camera_lock()
                first_lock.close()
                replacement_lock = acquire_camera_lock()
                replacement_lock.close()

    def test_camera_uses_stable_explicit_profiles(self) -> None:
        command, environment = camera_command()
        self.assertEqual(command[:4], ["ros2", "launch", "orbbec_camera", "gemini2.launch.py"])
        self.assertIn("color_width:=640", command)
        self.assertIn("depth_height:=400", command)
        self.assertIn("depth_format:=ANY", command)
        self.assertIn("enable_frame_sync:=false", command)
        self.assertIn("enable_accel:=true", command)
        self.assertIn("enable_gyro:=true", command)
        self.assertIn("enable_sync_output_accel_gyro:=true", command)
        self.assertEqual(load_yaml("config/camera/gemini2.yaml")["imu"]["topic"],
                         "/camera/gyro_accel/sample")
        self.assertIn("ROS_LOG_DIR", environment)

    def test_factory_intrinsics_have_no_local_override_path(self) -> None:
        obsolete_paths = (
            "apps/run_calibration.py",
            "apps/run_calibration_session.py",
            "config/calibration/chessboard.yaml",
            "scripts/calibrate_camera.sh",
            "tools/camera_calibration/calibrate_camera.py",
        )
        for relative_path in obsolete_paths:
            with self.subTest(relative_path=relative_path):
                self.assertFalse((REPOSITORY_ROOT / relative_path).exists())

    def test_detector_is_direct_executable_with_central_config(self) -> None:
        command = detector_command(require_built=False)
        self.assertEqual(Path(command[0]), REPOSITORY_ROOT / "install/lbot_vision/lib/lbot_vision/nut_detector_node")
        self.assertEqual(command[1:3], ["--ros-args", "--params-file"])
        self.assertEqual(Path(command[3]), REPOSITORY_ROOT / "config/vision/nut_detector.yaml")

    def test_viewer_resolves_ros_package_executable(self) -> None:
        command = viewer_command()
        self.assertTrue(command[0].endswith("/lib/rqt_image_view/rqt_image_view"))
        self.assertEqual(command[1], "/camera/color/image_raw")

    def test_configuration_has_one_central_source(self) -> None:
        config_files = sorted((REPOSITORY_ROOT / "config").rglob("*.yaml"))
        self.assertGreaterEqual(len(config_files), 5)
        for config_file in config_files:
            with self.subTest(config_file=config_file):
                self.assertIsInstance(load_yaml(config_file), dict)

        package = REPOSITORY_ROOT / "src/lbot_vision"
        self.assertFalse((package / "config").exists())
        self.assertFalse((package / "launch").exists())


if __name__ == "__main__":
    unittest.main()
