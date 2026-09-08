#!/usr/bin/env python3
"""Application entry point for the configured camera driver."""

from __future__ import annotations

import argparse
import os
import shlex
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from linkerbot.runtime import (  # noqa: E402
    ConfigurationError,
    acquire_camera_lock,
    camera_command,
)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", default="config/camera/gemini2.yaml")
    parser.add_argument("--dry-run", action="store_true")
    arguments = parser.parse_args()
    try:
        command, environment = camera_command(arguments.config)
    except ConfigurationError as error:
        parser.error(str(error))
    if arguments.dry_run:
        print(shlex.join(command))
        return 0
    try:
        camera_lock = acquire_camera_lock()
    except ConfigurationError as error:
        parser.error(str(error))
    # run_camera replaces this process with ros2; keep the lock descriptor open
    # across exec so it remains held for the complete launch lifetime.
    os.set_inheritable(camera_lock.fileno(), True)
    os.execvpe(command[0], command, environment)
    return 127


if __name__ == "__main__":
    raise SystemExit(main())
