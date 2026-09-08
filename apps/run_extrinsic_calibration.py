#!/usr/bin/env python3
"""Run the fixed-camera calibration tool through the workspace adapter."""

import os
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

from linkerbot.runtime import extrinsic_calibration_command


if __name__ == "__main__":
    command = extrinsic_calibration_command(sys.argv[1:])
    os.execv(command[0], command)
