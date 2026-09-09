#!/usr/bin/env bash
set -Eeuo pipefail

ARUCO_CALIBRATION_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export PYTHONPATH="${ARUCO_CALIBRATION_ROOT}/src${PYTHONPATH:+:${PYTHONPATH}}"
cd "${ARUCO_CALIBRATION_ROOT}"
exec python3 -m unittest discover -s tests -v
