"""Read the driver's internal TF and attach its root without reparenting optics."""

from __future__ import annotations

import time
from collections import defaultdict
from typing import Any, Mapping

import numpy as np

from .core import CalibrationError, matrix_from_pose
from .safety import validate_camera_tf_tree


def lookup_camera_internal_transform(config: Mapping[str, Any]) -> np.ndarray:
    try:
        import rclpy
        from rclpy.node import Node
        from rclpy.qos import DurabilityPolicy, QoSProfile, ReliabilityPolicy
        from rclpy.time import Time
        from tf2_msgs.msg import TFMessage
        from tf2_ros import Buffer, TransformListener
    except ImportError as error:
        raise CalibrationError("TF Python dependencies unavailable; source ROS and install tf2_ros/tf2_msgs") from error

    options = config["tf_publish"]
    root = str(options["camera_root_frame"])
    optical = str(config["frames"]["camera_frame"])
    base = str(config["frames"]["base_frame"])
    parents: dict[str, set[str]] = defaultdict(set)
    static_edges: set[tuple[str, str]] = set()

    def observe(message: Any, static: bool) -> None:
        for item in message.transforms:
            parent, child = str(item.header.frame_id), str(item.child_frame_id)
            parents[child].add(parent)
            if static:
                static_edges.add((parent, child))

    rclpy.init(args=None)
    node = Node("aruco_external_tf_preflight")
    listener = None
    try:
        buffer = Buffer()
        listener = TransformListener(buffer, node, spin_thread=False)
        static_qos = QoSProfile(depth=100, durability=DurabilityPolicy.TRANSIENT_LOCAL,
                               reliability=ReliabilityPolicy.RELIABLE)
        dynamic_qos = QoSProfile(depth=100, reliability=ReliabilityPolicy.BEST_EFFORT)
        node.create_subscription(TFMessage, "/tf_static", lambda msg: observe(msg, True), static_qos)
        node.create_subscription(TFMessage, "/tf", lambda msg: observe(msg, False), dynamic_qos)
        started = time.monotonic()
        deadline = started + float(options["lookup_timeout_s"])
        while rclpy.ok() and time.monotonic() < deadline:
            rclpy.spin_once(node, timeout_sec=0.05)
            if time.monotonic() - started < float(options["discovery_window_s"]):
                continue
            if buffer.can_transform(root, optical, Time()):
                validate_camera_tf_tree(parents, static_edges, root, optical, base)
                transform = buffer.lookup_transform(root, optical, Time()).transform
                t, q = transform.translation, transform.rotation
                return matrix_from_pose([t.x, t.y, t.z], [q.x, q.y, q.z, q.w])
        raise CalibrationError(
            f"cannot read internal static TF {root} <- {optical}; "
            "start the camera driver with publish_tf:=true and check frame names"
        )
    finally:
        if listener is not None:
            listener.unregister()
        node.destroy_node()
        rclpy.shutdown()
