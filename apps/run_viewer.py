#!/usr/bin/env python3
"""Open the configured live-image viewer without exposing ROS CLI details."""

from __future__ import annotations

import argparse
import os
import shlex
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from linkerbot.runtime import ConfigurationError, viewer_command  # noqa: E402


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--config", default="config/viewer/image.yaml")
    parser.add_argument("--topic", help="Temporarily override the configured image topic")
    parser.add_argument("--dry-run", action="store_true")
    arguments = parser.parse_args()
    try:
        command = viewer_command(arguments.config, topic=arguments.topic)
    except ConfigurationError as error:
        parser.error(str(error))
    if arguments.dry_run:
        print(shlex.join(command))
        return 0
    os.execv(command[0], command)
    return 127


if __name__ == "__main__":
    raise SystemExit(main())
