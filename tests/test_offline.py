from __future__ import annotations

import contextlib
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import yaml

from linkerbot.offline import (build, collect_frames, detector_parameters, executable, replay,
                              settings, source_version)
from linkerbot.runtime import ConfigurationError, REPOSITORY_ROOT


class OfflineConfigurationTests(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)

    def yaml_file(self, value):
        path = self.directory / "config.yaml"
        path.write_text(yaml.safe_dump(value), encoding="utf-8")
        return path

    def test_central_config_and_fields(self):
        config = settings("config/vision/offline.yaml")
        self.assertEqual(config["build_jobs"], 2)
        params = detector_parameters(config["detector_config"])
        self.assertEqual(params["black_v_max"], 90)
        self.assertEqual(params["min_frame_area_ratio"], 0.03)
        self.assertNotIn("depth_topic", params)
        self.assertEqual(len(params), 22)

    def test_missing_detector_field(self):
        path = self.yaml_file({"nut_detector_node": {"ros__parameters": {}}})
        with self.assertRaisesRegex(ConfigurationError, "black_v_max"):
            detector_parameters(path)

    def test_bad_scalar_types(self):
        config = settings("config/vision/offline.yaml")
        document = yaml.safe_load(config["detector_config"].read_text(encoding="utf-8"))
        for name, value in (("black_v_max", True), ("adaptive_block_size", 3.5),
                            ("hough_param2", float("nan")), ("basket_side", None)):
            with self.subTest(name=name):
                modified = {"nut_detector_node": {"ros__parameters": dict(document["nut_detector_node"]["ros__parameters"])}}
                modified["nut_detector_node"]["ros__parameters"][name] = value
                with self.assertRaisesRegex(ConfigurationError, name):
                    detector_parameters(self.yaml_file(modified))

    def test_offline_config_validation(self):
        original = yaml.safe_load((REPOSITORY_ROOT / "config/vision/offline.yaml").read_text(encoding="utf-8"))
        for key, value in (("build_jobs", 32), ("build_jobs", True), ("build_directory", "src"),
                           ("output_directory", "config"), ("detector_config", None)):
            with self.subTest(key=key, value=value):
                with self.assertRaises(ConfigurationError):
                    settings(self.yaml_file({**original, key: value}))
        with self.assertRaises(ConfigurationError):
            settings(self.yaml_file({**original, "typo": 1}))

    def test_directory_order_and_unknown_timestamps(self):
        for name in ("0002.PNG", "0001.jpg", "ignored.txt"):
            (self.directory / name).write_bytes(b"image")
        frames = collect_frames([str(self.directory)], None)
        self.assertEqual([f["source"].name for f in frames], ["0001.jpg", "0002.PNG"])
        self.assertTrue(all(f["capture_timestamp_ns"] is None for f in frames))

    def test_manifest_preserves_order_timestamps_and_metadata(self):
        for name in ("a.png", "b.png"):
            (self.directory / name).touch()
        manifest = self.directory / "sequence.json"
        manifest.write_text(json.dumps({"frames": [{"image": "b.png", "capture_timestamp_ns": 123,
                                                   "frame_id": "color"}, {"image": "a.png"}]}), encoding="utf-8")
        frames = collect_frames([], str(manifest))
        self.assertEqual(frames[0]["source"].name, "b.png")
        self.assertEqual(frames[0]["capture_timestamp_ns"], 123)
        self.assertEqual(frames[0]["metadata"]["frame_id"], "color")
        self.assertIsNone(frames[1]["capture_timestamp_ns"])
        with self.assertRaises(ConfigurationError):
            collect_frames(["also.png"], str(manifest))

    def test_manifest_rejects_bad_timestamp(self):
        image = self.directory / "a.png"
        image.touch()
        manifest = self.directory / "sequence.json"
        manifest.write_text(json.dumps({"frames": [{"image": "a.png", "capture_timestamp_ns": True}]}), encoding="utf-8")
        with self.assertRaisesRegex(ConfigurationError, "timestamp"):
            collect_frames([], str(manifest))

    def test_missing_or_empty_inputs(self):
        for inputs in ([], [str(self.directory)], [str(self.directory / "missing.png")]):
            with self.subTest(inputs=inputs), self.assertRaises(ConfigurationError):
                collect_frames(inputs, None)

    def test_dry_run_does_not_create_output(self):
        config = settings("config/vision/offline.yaml")
        with tempfile.TemporaryDirectory(dir=REPOSITORY_ROOT / "artifacts") as parent:
            output = Path(parent) / "not_created"
            image = self.directory / "a.png"
            image.touch()
            with contextlib.redirect_stdout(io.StringIO()):
                self.assertEqual(replay(config, collect_frames([str(image)], None), str(output), True), 0)
            self.assertFalse(output.exists())
            with self.assertRaisesRegex(ConfigurationError, "overwrite"):
                replay(config, collect_frames([str(image)], None), parent, True)

    def test_missing_binary_actionable(self):
        config = settings("config/vision/offline.yaml")
        config["build_directory"] = self.directory / "not_built"
        with self.assertRaisesRegex(ConfigurationError, "not built"):
            replay(config, [], None)

    def test_shared_core_linked_into_both_builds(self):
        ros = (REPOSITORY_ROOT / "src/lbot_vision/CMakeLists.txt").read_text(encoding="utf-8")
        standalone = (REPOSITORY_ROOT / "tools/offline_detection/CMakeLists.txt").read_text(encoding="utf-8")
        node = (REPOSITORY_ROOT / "src/lbot_vision/src/ros/nut_detector_node.cpp").read_text(encoding="utf-8")
        self.assertIn("include(detector_core.cmake)", ros)
        self.assertIn("detector_core.cmake", standalone)
        self.assertIn("lbot_vision::detect_2d", node)
        self.assertNotIn("cv::HoughCircles", node)
        self.assertLess(node.index("lbot_vision::detect_2d"), node.index("if (!depth_msg_"))
        for name in ("build_offline", "run_detector_offline", "test_offline"):
            script = (REPOSITORY_ROOT / f"scripts/{name}.sh").read_text(encoding="utf-8")
            self.assertNotIn('source "', script)
            self.assertIn("exec python3", script)
            self.assertTrue((REPOSITORY_ROOT / f"scripts/{name}.ps1").is_file())
        for path in (REPOSITORY_ROOT / "src/lbot_vision/src/core").glob("*.cpp"):
            self.assertNotIn("rclcpp", path.read_text(encoding="utf-8"))

    def test_build_uses_low_concurrency_and_no_ros(self):
        config = settings("config/vision/offline.yaml")
        config["build_directory"] = self.directory / "build"
        with patch("linkerbot.offline.run_command") as command:
            build(config)
        commands = [call.args[0] for call in command.call_args_list]
        self.assertEqual(commands[0][0], "cmake")
        self.assertEqual(commands[1][-2:], ["--parallel", "2"])
        self.assertFalse(any("ros2" in arg or "colcon" in arg for cmd in commands for arg in cmd))


class OfflineReplayIntegrationTests(unittest.TestCase):
    def test_real_binary_outputs_and_corrupt_frame(self):
        config = settings("config/vision/offline.yaml")
        if not executable(config).is_file():
            self.skipTest("Build standalone C++ detector to run real integration test")
        with tempfile.TemporaryDirectory(dir=REPOSITORY_ROOT / "artifacts") as parent:
            directory = Path(parent)
            # PPM bytes avoid a Python OpenCV dependency; filename intentionally non-ASCII.
            good = directory / "测试原图.ppm"
            good.write_bytes(b"P6\n160 120\n255\n" + bytes([240, 240, 240]) * 160 * 120)
            bad = directory / "bad.png"
            bad.write_bytes(b"not an image")
            output = directory / "result"
            with contextlib.redirect_stdout(io.StringIO()):
                code = replay(config, collect_frames([str(good), str(bad)], None), str(output))
            self.assertEqual(code, 1)
            record = json.loads((output / "run.json").read_text(encoding="utf-8"))
            self.assertEqual(record["state"], "completed_with_errors")
            self.assertEqual(len(record["frames"]), 2)
            result = json.loads((output / "000000/result.json").read_text(encoding="utf-8"))
            self.assertFalse(result["frame_found"])
            self.assertFalse(result["localization_valid"])
            self.assertIsNone(result["position"])
            self.assertIsNone(result["capture_timestamp_ns"])
            self.assertEqual((output / "000000/input.bin").read_bytes(), good.read_bytes())
            self.assertTrue((output / "000000/annotated.png").is_file())
            error = json.loads((output / "000001/result.json").read_text(encoding="utf-8"))
            self.assertEqual(error["status"], "processing_error")
            self.assertIsNotNone(record["binary_sha256"])
            self.assertTrue(record["version"]["source_sha256"])


if __name__ == "__main__":
    unittest.main()
