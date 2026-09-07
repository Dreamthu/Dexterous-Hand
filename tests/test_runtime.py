from __future__ import annotations

import unittest

from linkerbot.runtime import (
    REPOSITORY_ROOT,
    calibration_command,
    camera_command,
    detector_command,
    load_yaml,
    viewer_command,
)


class RuntimeConfigurationTests(unittest.TestCase):
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

    def test_detector_is_direct_executable_with_central_config(self) -> None:
        command = detector_command(require_built=False)
        self.assertTrue(command[0].endswith("/lbot_vision/nut_detector_node"))
        self.assertEqual(command[1:3], ["--ros-args", "--params-file"])
        self.assertTrue(command[3].endswith("/config/vision/nut_detector.yaml"))

    def test_viewer_resolves_ros_package_executable(self) -> None:
        command = viewer_command()
        self.assertTrue(command[0].endswith("/lib/rqt_image_view/rqt_image_view"))
        self.assertEqual(command[1], "/camera/color/image_raw")

    def test_calibration_uses_board_configuration(self) -> None:
        command = calibration_command()
        self.assertIn("--squares-x", command)
        self.assertEqual(command[command.index("--squares-x") + 1], "11")
        self.assertEqual(command[command.index("--squares-y") + 1], "8")
        self.assertEqual(command[command.index("--square-size-mm") + 1], "15.0")
        self.assertEqual(command[command.index("--min-board-area-ratio") + 1], "0.05")

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
