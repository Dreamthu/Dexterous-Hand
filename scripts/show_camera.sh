#!/usr/bin/env bash
set -Eeuo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# shellcheck source=_common.sh
source "${SCRIPT_DIR}/_common.sh"

if ! command -v rqt_image_view >/dev/null 2>&1; then
  echo "rqt_image_view is not installed. Install ros-jazzy-rqt-image-view." >&2
  exit 2
fi
exec rqt_image_view
