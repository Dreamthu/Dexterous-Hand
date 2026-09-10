#!/usr/bin/env bash
set -Eeuo pipefail

ARUCO_TEST_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
export PYTHONPATH="${ARUCO_TEST_ROOT}/src${PYTHONPATH:+:${PYTHONPATH}}"
# Fake service and fake frames only, isolated from the real robot's ROS graph.
export ROS_DOMAIN_ID="${ARUCO_TEST_DOMAIN_ID:-227}"
export ROS_AUTOMATIC_DISCOVERY_RANGE=LOCALHOST
export ROS_STATIC_PEERS=""
unset ROS_DISCOVERY_SERVER
cd "${ARUCO_TEST_ROOT}"
exec python3 tests/ros_smoke.py
