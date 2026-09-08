#!/usr/bin/env bash
set -Eeuo pipefail
# Deliberately do not source _common.sh: offline tools require no ROS/SDK.
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
exec python3 "${SCRIPT_DIR}/../apps/offline_detection.py" run "$@"
