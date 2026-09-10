"""The committed deployment baseline must work without ignored capture files."""

from contextlib import redirect_stdout
import io
import shutil
import subprocess
import unittest
from unittest.mock import patch

import numpy as np

from extrinsic_calibration.cli import PROJECT_ROOT, publish_command
from extrinsic_calibration.core import (
    load_config, load_yaml, training_quality, transform_from_dict, validation_quality,
)
from extrinsic_calibration.safety import robot_contract


class DeliveryTests(unittest.TestCase):
    def test_committed_result_matches_its_config_snapshot(self):
        config = load_config(PROJECT_ROOT / "artifacts/calibration_config_v2.yaml")
        result = load_yaml(PROJECT_ROOT / config["solve"]["result"])
        self.assertIs(result["quality"]["accepted"], True)
        self.assertEqual(result["frames"], config["frames"])
        self.assertEqual(result["robot_contract"], robot_contract(config))
        self.assertEqual(result["parent_frame"], config["frames"]["base_frame"])
        self.assertEqual(result["child_frame"], config["frames"]["camera_frame"])
        self.assertTrue(training_quality(result["training_metrics"], config["quality"])[0])
        self.assertTrue(validation_quality(result["validation_metrics"], config["quality"])[0])
        np.testing.assert_allclose(transform_from_dict(result),
                                   result["matrix_parent_from_child"], atol=1e-10)

    def test_deployment_does_not_read_raw_observations(self):
        config = load_config(PROJECT_ROOT / "artifacts/calibration_config_v2.yaml")
        output = io.StringIO()
        with patch("extrinsic_calibration.cli.load_json", side_effect=AssertionError("raw data dependency")), \
                patch("extrinsic_calibration.tf_publish.lookup_camera_internal_transform", return_value=np.eye(4)), \
                patch("extrinsic_calibration.cli.os.execvp") as execute, redirect_stdout(output):
            self.assertEqual(publish_command(config, dry_run=True), 0)
            execute.assert_not_called()
        self.assertIn("--frame-id base_link --child-frame-id camera_link", output.getvalue())

    def test_git_ignore_keeps_only_deployment_artifacts_and_shared_config(self):
        if not (PROJECT_ROOT / ".git").exists() or not shutil.which("git"):
            self.skipTest("Git metadata is not present in this source export")
        for relative, ignored in (
            ("artifacts/gemini2_to_base_v2.yaml", False),
            ("artifacts/calibration_config_v2.yaml", False),
            ("config/eye_to_hand_aruco.yaml", False),
            ("config/environment.example", False),
            ("src/extrinsic_calibration/tf_publish.py", False),
            ("artifacts/right_wrist_calibration_v2.json", True),
            ("artifacts/right_wrist_validation_v2_images/sample_001.png", True),
            ("artifacts/gemini2_to_base_session03.yaml", True),
            ("config/environment.local", True),
            ("config/next_session.yaml", True),
            ("tests/__pycache__/test_delivery.cpython-312.pyc", True),
            (".venv/pyvenv.cfg", True),
            ("build/output.log", True),
            ("unrelated.txt", True),
        ):
            with self.subTest(path=relative):
                result = subprocess.run(
                    ["git", "check-ignore", "--no-index", "--quiet", "--", relative],
                    cwd=PROJECT_ROOT, check=False,
                )
                self.assertEqual(result.returncode, 0 if ignored else 1)


if __name__ == "__main__":
    unittest.main()
