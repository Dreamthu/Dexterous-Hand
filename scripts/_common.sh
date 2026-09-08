#!/usr/bin/env bash

# Shared environment adapter. User-facing scripts source this file so ROS and
# external SDK details do not leak into normal application commands.

LINKERBOT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export LINKERBOT_ROOT

set -a
# shellcheck source=/dev/null
source "${LINKERBOT_ROOT}/config/workspace.env"
# Machine-specific paths belong in the ignored override, never shared defaults.
if [[ -f "${LINKERBOT_ROOT}/config/workspace.local.env" ]]; then
  # shellcheck source=/dev/null
  source "${LINKERBOT_ROOT}/config/workspace.local.env"
fi
set +a

linkerbot_resolve_path() {
  if [[ "$1" = /* ]]; then
    realpath -m "$1"
  else
    realpath -m "${LINKERBOT_ROOT}/$1"
  fi
}

LINKERBOT_ROS_SETUP="$(linkerbot_resolve_path "${LINKERBOT_ROS_SETUP}")"
LINKERBOT_CAMERA_WORKSPACE="$(linkerbot_resolve_path "${LINKERBOT_CAMERA_WORKSPACE}")"
LINKERBOT_ROBOT_WORKSPACE="$(linkerbot_resolve_path "${LINKERBOT_ROBOT_WORKSPACE}")"
export LINKERBOT_ROS_SETUP LINKERBOT_CAMERA_WORKSPACE LINKERBOT_ROBOT_WORKSPACE

if [[ ! -f "${LINKERBOT_ROS_SETUP}" ]]; then
  echo "ROS setup not found: ${LINKERBOT_ROS_SETUP}" >&2
  exit 2
fi

# ROS setup files are third-party shell code and may reference unset variables.
set +u
# shellcheck source=/dev/null
source "${LINKERBOT_ROS_SETUP}"
if [[ -f "${LINKERBOT_CAMERA_WORKSPACE}/install/setup.bash" ]]; then
  # shellcheck source=/dev/null
  source "${LINKERBOT_CAMERA_WORKSPACE}/install/setup.bash"
fi
if [[ -f "${LINKERBOT_ROOT}/install/setup.bash" ]]; then
  # shellcheck source=/dev/null
  source "${LINKERBOT_ROOT}/install/setup.bash"
fi
set -u

export PYTHONPATH="${LINKERBOT_ROOT}${PYTHONPATH:+:${PYTHONPATH}}"
