#!/usr/bin/env python3
"""Start the camera, run intrinsic calibration, then stop the camera."""

from __future__ import annotations

import argparse
import os
import signal
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from linkerbot.runtime import (  # noqa: E402
    ConfigurationError,
    calibration_command,
    camera_command,
)


def stop_process(process: subprocess.Popen[bytes]) -> None:
    if process.poll() is not None:
        return
    try:
        os.killpg(process.pid, signal.SIGINT)
        process.wait(timeout=8.0)
    except (ProcessLookupError, subprocess.TimeoutExpired):
        if process.poll() is None:
            os.killpg(process.pid, signal.SIGTERM)
            try:
                process.wait(timeout=3.0)
            except subprocess.TimeoutExpired:
                os.killpg(process.pid, signal.SIGKILL)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--camera-config", default="config/camera/gemini2.yaml")
    parser.add_argument("--calibration-config", default="config/calibration/chessboard.yaml")
    arguments = parser.parse_args()
    try:
        camera, camera_environment = camera_command(arguments.camera_config)
        calibration = calibration_command(arguments.calibration_config)
    except ConfigurationError as error:
        parser.error(str(error))

    print("Starting the configured camera...", flush=True)
    camera_process = subprocess.Popen(
        camera, env=camera_environment, start_new_session=True
    )
    try:
        time.sleep(4.0)
        if camera_process.poll() is not None:
            print("Camera exited before calibration could start.", file=sys.stderr)
            return camera_process.returncode or 1
        print("Opening the calibration window...", flush=True)
        calibration_process = subprocess.Popen(calibration, start_new_session=True)
        try:
            return calibration_process.wait()
        except KeyboardInterrupt:
            stop_process(calibration_process)
            return 130
    finally:
        stop_process(camera_process)


if __name__ == "__main__":
    raise SystemExit(main())
