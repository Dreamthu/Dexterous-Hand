#!/usr/bin/env bash
set -Eeuo pipefail

ARUCO_CALIBRATION_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
export ARUCO_CALIBRATION_ROOT

ARUCO_ENV_FILE="${ARUCO_CALIBRATION_ENV_FILE:-${ARUCO_CALIBRATION_ROOT}/config/environment.local}"
if [[ -f "${ARUCO_ENV_FILE}" ]]; then
  set -a
  # shellcheck source=/dev/null
  source "${ARUCO_ENV_FILE}"
  set +a
fi

for setup_variable in \
  ARUCO_CALIBRATION_ROS_SETUP \
  ARUCO_CALIBRATION_CAMERA_SETUP \
  ARUCO_CALIBRATION_ROBOT_SETUP; do
  setup_path="${!setup_variable:-}"
  if [[ -n "${setup_path}" ]]; then
    if [[ ! -f "${setup_path}" ]]; then
      echo "Configured setup file does not exist (${setup_variable}): ${setup_path}" >&2
      exit 2
    fi
    # Third-party ROS setup scripts may reference unset variables.
    set +u
    # shellcheck source=/dev/null
    source "${setup_path}"
    set -u
  fi
done

export PYTHONPATH="${ARUCO_CALIBRATION_ROOT}/src${PYTHONPATH:+:${PYTHONPATH}}"
cd "${ARUCO_CALIBRATION_ROOT}"
exec python3 scripts/run_calibration.py "$@"
