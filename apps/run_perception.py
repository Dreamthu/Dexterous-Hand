#!/usr/bin/env python3
"""Run the configured camera and detector as one managed application."""

from __future__ import annotations

import argparse
import os
import signal
import subprocess
import sys
import time
from pathlib import Path
from typing import List

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from linkerbot.runtime import (  # noqa: E402
    ConfigurationError,
    acquire_camera_lock,
    camera_command,
    detector_command,
)


def stop_process(process: subprocess.Popen[bytes], timeout_seconds: float = 8.0) -> None:
    if process.poll() is not None:
        return
    try:
        os.killpg(process.pid, signal.SIGINT)
        process.wait(timeout=timeout_seconds)
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
    parser.add_argument("--vision-config", default="config/vision/nut_detector.yaml")
    arguments = parser.parse_args()
    try:
        camera, camera_environment = camera_command(arguments.camera_config)
        detector = detector_command(arguments.vision_config)
        camera_lock = acquire_camera_lock()
    except ConfigurationError as error:
        parser.error(str(error))

    processes: List[subprocess.Popen[bytes]] = []
    return_code = 0
    try:
        print("Starting camera adapter...", flush=True)
        processes.append(
            subprocess.Popen(camera, env=camera_environment, start_new_session=True)
        )
        time.sleep(3.0)
        if processes[0].poll() is not None:
            return processes[0].returncode or 1

        print("Starting perception adapter...", flush=True)
        processes.append(subprocess.Popen(detector, start_new_session=True))
        while True:
            for process in processes:
                code = process.poll()
                if code is not None:
                    return_code = code
                    return return_code
            time.sleep(0.25)
    except KeyboardInterrupt:
        return_code = 130
    finally:
        for process in reversed(processes):
            stop_process(process)
    return return_code


if __name__ == "__main__":
    raise SystemExit(main())
