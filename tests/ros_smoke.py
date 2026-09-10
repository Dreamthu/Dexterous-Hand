"""Optional real ROS adapter smoke test, using only an in-process fake robot/camera.

Run scripts/test_ros.sh after sourcing ROS and the installed robot interface overlay.
Unlike the geometry tests, this needs ROS but does not need hardware or a display.
"""

from __future__ import annotations

import copy
import threading
import time
from unittest.mock import patch

import numpy as np
import rclpy
from geometry_msgs.msg import TransformStamped
from lbot_arm_interfaces.srv import GetCurrentFrame
from rclpy.executors import SingleThreadedExecutor
from rclpy.node import Node
from tf2_ros import StaticTransformBroadcaster

from extrinsic_calibration.cli import DEFAULT_CONFIG
from extrinsic_calibration.core import CalibrationError, load_config
from extrinsic_calibration.tf_publish import lookup_camera_internal_transform
from extrinsic_calibration.tool_monitor import ToolMonitor


def main() -> None:
    rclpy.init()
    server = Node("aruco_smoke_server")
    broadcaster = StaticTransformBroadcaster(server)
    links = []
    for parent, child, x in (("smoke_root", "smoke_color", .02),
                             ("smoke_color", "smoke_optical", .01)):
        message = TransformStamped()
        message.header.frame_id, message.child_frame_id = parent, child
        message.transform.rotation.w = 1.0
        message.transform.translation.x = x
        links.append(message)
    broadcaster.sendTransform(links)
    changed = threading.Event()

    def tool(_request, response):
        response.success = True
        response.frame.name = "ChangedTool" if changed.is_set() else "Arm_Tip"
        return response

    server.create_service(GetCurrentFrame, "/aruco_smoke/get_tool", tool)
    executor = SingleThreadedExecutor()
    executor.add_node(server)
    thread = threading.Thread(target=executor.spin)
    thread.start()
    client = None
    try:
        config = load_config(DEFAULT_CONFIG)
        config["frames"]["base_frame"] = "smoke_base"
        config["frames"]["camera_frame"] = "smoke_optical"
        config["tf_publish"]["camera_root_frame"] = "smoke_root"
        # Share the test's context; subscriptions, buffer and service calls are real.
        with patch("rclpy.init"), patch("rclpy.shutdown"):
            internal = lookup_camera_internal_transform(config)
        np.testing.assert_allclose(internal[:3, 3], [.03, 0, 0], atol=1e-12)
        print("PASS: real ROS TF subscriptions, buffer lookup and topology checks", flush=True)

        client = Node("aruco_smoke_client")
        robot = copy.deepcopy(config["robot"])
        robot["tool_service"] = "/aruco_smoke/get_tool"
        monitor = ToolMonitor(client, robot)
        deadline = time.monotonic() + 6.0
        checked = False
        while time.monotonic() < deadline:
            rclpy.spin_once(client, timeout_sec=.02)
            monitor.poll()
            try:
                monitor.require_current()
                checked = True
                break
            except CalibrationError:
                pass
        assert checked, monitor.error
        print("PASS: real GetCurrentFrame service request/response", flush=True)

        changed.set()
        deadline = time.monotonic() + 5.0
        while time.monotonic() < deadline and not monitor.latched_error:
            rclpy.spin_once(client, timeout_sec=.02)
            monitor.poll()
        assert monitor.latched_error, "simulated tool change was not blocked"
        print("PASS: tool change latches capture refusal", flush=True)
    finally:
        executor.shutdown()
        thread.join()
        if client is not None:
            client.destroy_node()
        server.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
