#!/usr/bin/env python3
"""Application entry point for nut and basket perception."""

from __future__ import annotations

import argparse
import os
import shlex
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from linkerbot.runtime import ConfigurationError, detector_command  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", default="config/vision/nut_detector.yaml")
    parser.add_argument("--dry-run", action="store_true")
    arguments = parser.parse_args()
    try:
        command = detector_command(arguments.config, require_built=not arguments.dry_run)
    except ConfigurationError as error:
        parser.error(str(error))
    if arguments.dry_run:
        print(shlex.join(command))
        return 0
    os.execv(command[0], command)
    return 127


if __name__ == "__main__":
    raise SystemExit(main())
