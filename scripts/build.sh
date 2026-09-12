#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=_common.sh
source "${SCRIPT_DIR}/_common.sh"

BUILD_JOBS="${LINKERBOT_BUILD_JOBS:-2}"
export MAKEFLAGS="-j${BUILD_JOBS} -l${BUILD_JOBS}"
export CMAKE_BUILD_PARALLEL_LEVEL="${BUILD_JOBS}"

# A copied or moved workspace contains CMake caches that still point to the old
# source/build paths. Reconfigure those builds without deleting the whole build
# tree.
CMAKE_CACHE_OPTIONS=()
for cmake_cache in "${LINKERBOT_ROOT}"/build/*/CMakeCache.txt; do
  [[ -f "${cmake_cache}" ]] || continue
  cache_home="$(grep -m1 '^CMAKE_HOME_DIRECTORY:INTERNAL=' "${cmake_cache}" | cut -d= -f2-)"
  if [[ -n "${cache_home}" && "${cache_home}" != "${LINKERBOT_ROOT}/src/$(basename "$(dirname "${cmake_cache}")")" ]]; then
    echo "Stale CMake cache detected: ${cmake_cache} (was created for ${cache_home})"
    echo "Reconfiguring CMake packages for the current workspace path."
    CMAKE_CACHE_OPTIONS=(--cmake-clean-cache)
    break
  fi
done

cd "${LINKERBOT_ROOT}"
colcon build \
  --base-paths "${LINKERBOT_ROOT}/src" \
  --symlink-install \
  --executor sequential \
  "${CMAKE_CACHE_OPTIONS[@]}" \
  --cmake-args -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF

echo
echo "Build complete. Run ./scripts/run_perception.sh to start the camera and detector."
