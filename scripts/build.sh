#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=_common.sh
source "${SCRIPT_DIR}/_common.sh"

ROBOT_INTERFACE_SOURCE="${LINKERBOT_ROBOT_WORKSPACE}/src/lbot_arm_interfaces"
if [[ ! -f "${ROBOT_INTERFACE_SOURCE}/package.xml" ]]; then
  echo "Robot interface package not found: ${ROBOT_INTERFACE_SOURCE}" >&2
  exit 2
fi

BUILD_JOBS="${LINKERBOT_BUILD_JOBS:-2}"
export MAKEFLAGS="-j${BUILD_JOBS} -l${BUILD_JOBS}"

cd "${LINKERBOT_ROOT}"
colcon build \
  --base-paths "${LINKERBOT_ROOT}/src" "${ROBOT_INTERFACE_SOURCE}" \
  --symlink-install \
  --executor sequential \
  --event-handlers console_direct+ \
  --cmake-args -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF

echo
echo "Build complete. Run ./scripts/run_perception.sh to start the camera and detector."
