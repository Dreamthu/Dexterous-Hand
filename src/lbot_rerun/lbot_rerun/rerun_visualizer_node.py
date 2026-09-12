"""Read-only ROS 2 visualizer for the LinkerBot workcell.

The node deliberately has no clients, service calls, or command publishers.
It can therefore run alongside the competition controller without changing
its behavior.  Rerun is imported lazily so the package can still be discovered
and its help text can be inspected on machines where the optional dependency
has not been installed yet.
"""

from __future__ import annotations

from dataclasses import dataclass
import json
import math
from pathlib import Path
import threading
from typing import Any

import numpy as np

from .core import (
    JointSpec,
    UrdfModel,
    downsample_points,
    interpolate_hand_command,
    joint_transform,
    parse_left_urdf,
    resolve_joint_values,
)


@dataclass
class _CameraInfo:
    width: int = 0
    height: int = 0
    fx: float = 0.0
    fy: float = 0.0
    cx: float = 0.0
    cy: float = 0.0
    frame_id: str = ""


@dataclass
class _HandState:
    command: tuple[int, ...] | None = None
    mode: str = "unknown"


class RerunVisualizerNode:
    """ROS subscriptions plus a small, defensive Rerun logging adapter."""

    def __init__(self, node: Any) -> None:
        self.node = node
        self._rr = self._import_rerun()
        self._lock = threading.RLock()
        self._frame_sequence = 0
        self._last_joint_values: dict[str, float] = {}
        self._last_joint_stamp_ns = -1
        self._last_cloud_stamp_ns = -1
        self._last_detection_stamp_ns = -1
        self._last_camera_info = _CameraInfo()
        self._hand_state = _HandState()
        self._warned: set[str] = set()
        self._camera_transform_logged = False

        self.robot_namespace = self._parameter("robot_namespace", "robot1")
        self.base_frame = self._parameter("base_frame", "base_link")
        self.urdf_path = Path(self._parameter("urdf_path", ""))
        self.model = self._load_model()
        self.model_prefix = self._parameter("model_entity", "world/base_link/robot/left")
        self.camera_entity = self._parameter("camera_entity", "world/base_link/camera")
        self.pointcloud_entity = self._parameter("pointcloud_entity", "world/base_link/perception/point_cloud")
        self.max_cloud_points = int(self._parameter("max_cloud_points", 50000))
        self.cloud_stride = max(1, int(self._parameter("cloud_stride", 1)))
        self.joint_topic = self._parameter(
            "joint_topic", f"/{self.robot_namespace.strip('/')}/left_arm/joint_states"
        )
        self.pose_topic = self._parameter(
            "pose_topic", f"/{self.robot_namespace.strip('/')}/left_arm/pose_states"
        )
        self.hand_topic = self._parameter(
            "hand_topic", f"/{self.robot_namespace.strip('/')}/left_hand/set_l6_joint"
        )
        self.detection_topic = self._parameter("detection_topic", "/nut_detections")
        self.sequence_topic = self._parameter("sequence_topic", "/nut_detections/sequence")
        self.slots_topic = self._parameter("slots_topic", "/nut_slots")
        self.pointcloud_topic = self._parameter("pointcloud_topic", "/camera/depth/color/points")
        self.camera_info_topic = self._parameter("camera_info_topic", "/camera/color/camera_info")
        self.color_topic = self._parameter("color_topic", "/camera/color/image_raw")
        self.tf_timeout_s = float(self._parameter("tf_timeout_s", 0.05))
        self.publish_cloud = bool(self._parameter("publish_cloud", True))
        self.log_camera_image = bool(self._parameter("log_camera_image", False))
        self.log_pose_state = bool(self._parameter("log_pose_state", True))
        self.hand_open = tuple(int(v) for v in self._parameter("hand_open", [255, 40, 255, 255, 255, 255]))
        raw_hand_closed = [
            int(value)
            for value in self._parameter(
                "hand_closed",
                [
                    150, 80, 150, 150, 150, 150,
                    165, 90, 165, 165, 165, 165,
                    180, 100, 180, 180, 180, 180,
                ],
            )
        ]
        if len(raw_hand_closed) == 6:
            raw_hand_closed *= 3
        if len(raw_hand_closed) != 18:
            raise ValueError("hand_closed must contain six or eighteen values")
        self.hand_closed = tuple(
            tuple(raw_hand_closed[offset : offset + 6])
            for offset in (0, 6, 12)
        )

        self._tf_buffer = None
        self._tf_listener = None
        self._subscriptions: list[Any] = []
        self._setup_ros()
        self._setup_recording()
        self._log_static_scene()

    @staticmethod
    def _import_rerun() -> Any:
        try:
            import rerun as rr
        except ImportError as error:  # pragma: no cover - environment-specific
            raise RuntimeError(
                "Rerun SDK is not installed. Run: python3 -m pip install --user --break-system-packages "
                "'rerun-sdk>=0.19,<0.20'"
            ) from error
        return rr

    def _parameter(self, name: str, default: Any) -> Any:
        self.node.declare_parameter(name, default)
        return self.node.get_parameter(name).value

    def _load_model(self) -> UrdfModel:
        if not self.urdf_path:
            try:
                from ament_index_python.packages import get_package_share_directory

                candidate = Path(get_package_share_directory("lbot_rerun")) / "开发资源/assets/workstations/lkls73_i1_o6_bimanual/workstation.urdf"
            except Exception:
                candidate = Path(__file__).resolve().parents[3] / "开发资源/assets/workstations/lkls73_i1_o6_bimanual/workstation.urdf"
            self.urdf_path = candidate
        return parse_left_urdf(self.urdf_path, root_link="base_base_link", root_alias=self.base_frame)

    def _setup_ros(self) -> None:
        from geometry_msgs.msg import PoseArray, PoseStamped
        from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy
        from sensor_msgs.msg import CameraInfo, Image, JointState, PointCloud2
        from std_msgs.msg import UInt8MultiArray

        sensor_qos = QoSProfile(
            depth=10,
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
        )
        reliable_qos = QoSProfile(depth=10, reliability=ReliabilityPolicy.RELIABLE)
        self._subscriptions.extend(
            [
                self.node.create_subscription(JointState, self.joint_topic, self._joint_callback, sensor_qos),
                self.node.create_subscription(PoseStamped, self.pose_topic, self._pose_callback, sensor_qos),
                self.node.create_subscription(UInt8MultiArray, self.hand_topic, self._hand_callback, reliable_qos),
                self.node.create_subscription(PoseArray, self.detection_topic, self._detection_callback, reliable_qos),
                self.node.create_subscription(PoseArray, self.slots_topic, self._slots_callback, reliable_qos),
                self.node.create_subscription(PointCloud2, self.pointcloud_topic, self._pointcloud_callback, sensor_qos),
                self.node.create_subscription(CameraInfo, self.camera_info_topic, self._camera_info_callback, reliable_qos),
            ]
        )
        # The optional streams are intentionally independent: absence of a
        # debug image or sequence message must not hide the 3D scene.
        if self.log_camera_image:
            self._subscriptions.append(
                self.node.create_subscription(Image, self.color_topic, self._image_callback, sensor_qos)
            )
        try:
            from lbot_vision.msg import NutSequenceState

            self._subscriptions.append(
                self.node.create_subscription(NutSequenceState, self.sequence_topic, self._sequence_callback, reliable_qos)
            )
        except ImportError:
            self._warn_once("sequence_import", "lbot_vision messages are unavailable; sequence timeline logging disabled")
        try:
            from tf2_ros import Buffer, TransformListener

            self._tf_buffer = Buffer()
            self._tf_listener = TransformListener(self._tf_buffer, self.node)
            self.node.create_timer(1.0, self._camera_timer)
        except Exception as error:  # pragma: no cover - ROS installation-specific
            self._warn_once("tf_setup", f"TF listener unavailable: {error}")

    def _setup_recording(self) -> None:
        self._recording_path = str(self._parameter("recording_path", ""))
        self._rr.init(self._parameter("application_id", "lbot_task"), spawn=False)
        viewer_mode = str(self._parameter("viewer_mode", "spawn"))
        if viewer_mode == "spawn":
            try:
                self._rr.spawn(
                    port=int(self._parameter("viewer_port", 9876)),
                    hide_welcome_screen=True,
                )
            except Exception as error:
                self._warn_once("viewer_spawn", f"Rerun viewer did not start: {error}")
        elif viewer_mode == "connect":
            self._rr.connect(self._parameter("viewer_url", "rerun+http://127.0.0.1:9876/proxy"))
        elif viewer_mode != "none":
            raise ValueError("viewer_mode must be spawn, connect, or none")
        self._rr.log("world", self._rr.ViewCoordinates.FLU, static=True)
        self._rr.log(
            "world/base_link",
            self._rr.Transform3D(translation=[0.0, 0.0, 0.0], axis_length=0.12),
            static=True,
        )

    def _warn_once(self, key: str, message: str) -> None:
        if key in self._warned:
            return
        self._warned.add(key)
        self.node.get_logger().warning(message)

    @staticmethod
    def _stamp_ns(message: Any) -> int:
        stamp = message.header.stamp
        return int(stamp.sec) * 1_000_000_000 + int(stamp.nanosec)

    def _set_time(self, stamp_ns: int) -> None:
        self._frame_sequence += 1
        self._rr.set_time_sequence("frame", self._frame_sequence)
        if stamp_ns > 0:
            self._rr.set_time_nanos("ros_time", stamp_ns)

    def _lookup_transform(self, source_frame: str, stamp_ns: int) -> Any | None:
        if not source_frame or source_frame == self.base_frame:
            return None
        if self._tf_buffer is None:
            self._warn_once("tf_missing", "TF listener is unavailable; skipping non-base-frame data")
            return False
        try:
            from rclpy.duration import Duration
            from rclpy.time import Time

            time = Time(nanoseconds=stamp_ns) if stamp_ns > 0 else Time()
            return self._tf_buffer.lookup_transform(
                self.base_frame,
                source_frame,
                time,
                timeout=Duration(seconds=self.tf_timeout_s),
            )
        except Exception as error:
            self._warn_once(
                f"tf:{source_frame}",
                f"skipping data in {source_frame}: no {self.base_frame} transform at its capture time ({error})",
            )
            return False

    def _transform_pose(self, message: Any, stamp_ns: int) -> Any | None:
        source_frame = str(message.header.frame_id) or self.base_frame
        transform = self._lookup_transform(source_frame, stamp_ns)
        if transform is False:
            return None
        if transform is None:
            return message
        try:
            from tf2_geometry_msgs import do_transform_pose_stamped

            return do_transform_pose_stamped(message, transform)
        except Exception as error:
            self._warn_once("pose_transform", f"could not transform pose from {source_frame}: {error}")
            return None

    def _transform_pose_array(self, message: Any, stamp_ns: int) -> np.ndarray | None:
        source_frame = str(message.header.frame_id) or self.base_frame
        transform = self._lookup_transform(source_frame, stamp_ns)
        if transform is False:
            return None
        if transform is None:
            return self._pose_positions(message)
        try:
            from geometry_msgs.msg import PointStamped
            from tf2_geometry_msgs import do_transform_point

            positions = []
            for pose in message.poses:
                point = PointStamped()
                point.header = message.header
                point.point = pose.position
                transformed = do_transform_point(point, transform)
                positions.append(
                    [transformed.point.x, transformed.point.y, transformed.point.z]
                )
            return np.asarray(positions, dtype=np.float32)
        except Exception as error:
            self._warn_once("pose_array_transform", f"could not transform PoseArray from {source_frame}: {error}")
            return None

    def _transform_cloud(self, message: Any, stamp_ns: int) -> Any | None:
        source_frame = str(message.header.frame_id) or self.base_frame
        transform = self._lookup_transform(source_frame, stamp_ns)
        if transform is False:
            return None
        if transform is None:
            return message
        try:
            from tf2_sensor_msgs.tf2_sensor_msgs import do_transform_cloud

            return do_transform_cloud(message, transform)
        except Exception as error:
            self._warn_once("cloud_transform", f"could not transform PointCloud2 from {source_frame}: {error}")
            return None

    def _camera_timer(self) -> None:
        info = self._last_camera_info
        frame = info.frame_id
        if frame:
            self._update_camera_transform(self.node.get_clock().now().nanoseconds, frame)

    def _update_camera_transform(self, stamp_ns: int, camera_frame: str) -> None:
        if self._camera_transform_logged:
            return
        transform = self._lookup_transform(camera_frame or self.base_frame, stamp_ns)
        if transform is False:
            return
        if transform is None:
            translation = [0.0, 0.0, 0.0]
            quaternion = [0.0, 0.0, 0.0, 1.0]
        else:
            translation = [
                transform.transform.translation.x,
                transform.transform.translation.y,
                transform.transform.translation.z,
            ]
            quaternion = [
                transform.transform.rotation.x,
                transform.transform.rotation.y,
                transform.transform.rotation.z,
                transform.transform.rotation.w,
            ]
        self._rr.log(
            self.camera_entity,
            self._rr.Transform3D(
                translation=translation,
                quaternion=self._rr.Quaternion(xyzw=quaternion),
                axis_length=0.08,
            ),
            static=True,
        )
        self._camera_transform_logged = True

    def _log_static_scene(self) -> None:
        # The Rerun URDF helper supports the full model, but the generated
        # workstation links use prefixed names.  Logging the selected links
        # through Asset3D keeps the physical root alias explicit and avoids
        # adding the unused right-arm branch to the task view.
        for link_name, link in self.model.links.items():
            entity = self.model.link_path(self.model_prefix, link_name)
            for index, visual in enumerate(link.visuals):
                visual_entity = f"{entity}/visual_{index}"
                try:
                    self._rr.log(
                        visual_entity,
                        self._rr.Asset3D(path=str(visual.mesh_path), media_type="model/stl"),
                        static=True,
                    )
                    if visual.origin_xyz != (0.0, 0.0, 0.0) or visual.origin_quaternion != (0.0, 0.0, 0.0, 1.0):
                        self._rr.log(
                            visual_entity,
                            self._rr.Transform3D(
                                translation=list(visual.origin_xyz),
                                quaternion=self._rr.Quaternion(xyzw=list(visual.origin_quaternion)),
                                static=True,
                            ),
                        )
                except Exception as error:
                    self._warn_once(f"mesh:{visual.mesh_path}", f"could not log mesh {visual.mesh_path}: {error}")

        # Log all fixed parent-child transforms.  Dynamic joints are updated
        # from JointState callbacks below.
        for joint in self.model.joints:
            if joint.joint_type != "fixed":
                continue
            self._log_joint_transform(joint, 0.0, static=True)
        self._set_time(self.node.get_clock().now().nanoseconds)
        for joint in self.model.movable_joints:
            self._log_joint_transform(joint, 0.0)

    def _log_joint_transform(self, joint: JointSpec, value: float, *, static: bool = False) -> None:
        translation, quaternion = joint_transform(joint, value)
        entity = self.model.link_path(self.model_prefix, joint.child_link)
        self._rr.log(
            entity,
            self._rr.Transform3D(
                translation=list(translation),
                quaternion=self._rr.Quaternion(xyzw=list(quaternion)),
            ),
            static=static,
        )

    def _joint_callback(self, message: Any) -> None:
        if len(message.name) == 0 or len(message.position) == 0:
            return
        reported = {
            str(name): float(value)
            for name, value in zip(message.name, message.position)
            if math.isfinite(float(value))
        }
        values = {name: reported[name] for name in self.model.arm_joint_names if name in reported}
        if not values and len(message.position) >= len(self.model.arm_joint_names):
            fallback = [float(value) for value in message.position[: len(self.model.arm_joint_names)]]
            if all(math.isfinite(value) for value in fallback):
                values = dict(zip(self.model.arm_joint_names, fallback))
                self._warn_once(
                    "joint_name_fallback",
                    "left joint-state names do not match the URDF; mapping the first seven positions by order",
                )
        if not values:
            return
        with self._lock:
            stamp_ns = self._stamp_ns(message)
            if stamp_ns and stamp_ns < self._last_joint_stamp_ns:
                return
            self._last_joint_stamp_ns = stamp_ns
            self._last_joint_values.update(values)
            resolved = resolve_joint_values(self.model, self._last_joint_values)
            self._set_time(stamp_ns)
            for joint in self.model.movable_joints:
                self._log_joint_transform(joint, resolved.get(joint.name, 0.0))
            if self.log_pose_state:
                self._rr.log(
                    "task/robot_joints",
                    self._rr.TextLog(json.dumps({name: resolved.get(name, 0.0) for name in self.model.arm_joint_names})),
                )

    def _pose_callback(self, message: Any) -> None:
        stamp_ns = self._stamp_ns(message)
        transformed = self._transform_pose(message, stamp_ns)
        if transformed is None:
            return
        pose = transformed.pose
        with self._lock:
            self._set_time(stamp_ns)
            self._rr.log(
                "world/base_link/robot_state/end_effector",
                self._rr.Points3D(
                    [[pose.position.x, pose.position.y, pose.position.z]],
                    radii=[0.018],
                    colors=[[240, 170, 30]],
                    labels=["left TCP"],
                    show_labels=True,
                ),
            )

    def _hand_callback(self, message: Any) -> None:
        command = tuple(int(value) for value in message.data)
        if len(command) != 6:
            self._warn_once("hand_length", "ignoring O6 command with a length other than six")
            return
        distances = [
            sum((a - b) ** 2 for a, b in zip(command, candidate))
            for candidate in (self.hand_open, *self.hand_closed)
        ]
        mode = ("open", "large_closed", "medium_closed", "small_closed")[int(np.argmin(distances))]
        self._hand_state = _HandState(command=command, mode=mode)
        closed = self.hand_open if mode == "open" else self.hand_closed[int(np.argmin(distances)) - 1]
        hand_limits = (
            (0.0, 1.30),
            (0.0, 0.58),
            (0.0, 1.60),
            (0.0, 1.60),
            (0.0, 1.60),
            (0.0, 1.60),
        )
        proxy = interpolate_hand_command(command, self.hand_open, closed, hand_limits)
        names = (
            "hand_left_lh_thumb_cmc_yaw",
            "hand_left_lh_thumb_cmc_pitch",
            "hand_left_lh_index_mcp_pitch",
            "hand_left_lh_middle_mcp_pitch",
            "hand_left_lh_ring_mcp_pitch",
            "hand_left_lh_pinky_mcp_pitch",
        )
        with self._lock:
            self._last_joint_values.update(dict(zip(names, proxy.values())))
            resolved = resolve_joint_values(self.model, self._last_joint_values)
            self._set_time(self.node.get_clock().now().nanoseconds)
            for name in names:
                joint = self.model.joint_by_name[name]
                self._log_joint_transform(joint, resolved[name])
            for joint in self.model.movable_joints:
                if joint.mimic_joint is not None:
                    self._log_joint_transform(joint, resolved[joint.name])
            self._rr.log("task/hand", self._rr.TextLog(f"O6 {mode}: {list(command)}"))

    @staticmethod
    def _pose_positions(message: Any) -> np.ndarray:
        return np.asarray(
            [[pose.position.x, pose.position.y, pose.position.z] for pose in message.poses],
            dtype=np.float32,
        )

    def _detection_callback(self, message: Any) -> None:
        self._log_pose_array(
            message,
            "world/base_link/perception/nuts",
            [255, 150, 30],
            ["large", "medium", "small"],
        )

    def _slots_callback(self, message: Any) -> None:
        self._log_pose_array(
            message,
            "world/base_link/perception/slots",
            [40, 130, 255],
            [f"slot {i + 1}" for i in range(len(message.poses))],
        )

    def _log_pose_array(self, message: Any, entity: str, color: list[int], labels: list[str]) -> None:
        stamp_ns = self._stamp_ns(message)
        positions = self._transform_pose_array(message, stamp_ns)
        if positions is None:
            return
        if positions.size == 0:
            # Preserve a clear visual state when the detector intentionally
            # publishes an empty PoseArray during an invalid observation.
            self._rr.log(entity, self._rr.Clear.flat())
            return
        with self._lock:
            self._set_time(stamp_ns)
            self._rr.log(
                entity,
                self._rr.Points3D(
                    positions,
                    radii=[0.015] * len(positions),
                    colors=[color] * len(positions),
                    labels=labels[: len(positions)],
                    show_labels=True,
                ),
            )

    def _pointcloud_callback(self, message: Any) -> None:
        if not self.publish_cloud:
            return
        from sensor_msgs_py import point_cloud2

        stamp_ns = self._stamp_ns(message)
        cloud = self._transform_cloud(message, stamp_ns)
        if cloud is None:
            return
        try:
            points = point_cloud2.read_points_numpy(
                cloud, field_names=["x", "y", "z"], skip_nans=True
            )
            points = np.asarray(points, dtype=np.float32)
            if self.cloud_stride > 1:
                points = points[:: self.cloud_stride]
            points = downsample_points(points, self.max_cloud_points)
        except Exception as error:
            self._warn_once("pointcloud_decode", f"could not decode PointCloud2: {error}")
            return
        if points.size == 0:
            return
        with self._lock:
            if stamp_ns and stamp_ns == self._last_cloud_stamp_ns:
                return
            self._last_cloud_stamp_ns = stamp_ns
            self._set_time(stamp_ns)
            self._rr.log(
                self.pointcloud_entity,
                self._rr.Points3D(points, radii=[0.0015] * len(points), colors=[150, 170, 185]),
            )
            self._rr.log(
                f"{self.pointcloud_entity}/source_frame",
                self._rr.TextLog(f"frame={self.base_frame}, points={len(points)}"),
            )

    def _camera_info_callback(self, message: Any) -> None:
        if len(message.k) != 9:
            return
        self._last_camera_info = _CameraInfo(
            width=int(message.width),
            height=int(message.height),
            fx=float(message.k[0]),
            fy=float(message.k[4]),
            cx=float(message.k[2]),
            cy=float(message.k[5]),
            frame_id=str(message.header.frame_id),
        )
        info = self._last_camera_info
        if info.width <= 0 or info.height <= 0 or info.fx <= 0 or info.fy <= 0:
            return
        self._rr.log(
            self.camera_entity,
            self._rr.Pinhole(
                focal_length=[info.fx, info.fy],
                principal_point=[info.cx, info.cy],
                resolution=[info.width, info.height],
                camera_xyz=self._rr.ViewCoordinates.RDF,
                image_plane_distance=0.25,
            ),
            static=True,
        )
        self._update_camera_transform(self._stamp_ns(message), info.frame_id)
        self._rr.log(
            f"{self.camera_entity}/frame",
            self._rr.Transform3D(axis_length=0.08),
            static=True,
        )

    def _image_callback(self, message: Any) -> None:
        try:
            from sensor_msgs_py import image as image_helper

            image = np.asarray(image_helper.imgmsg_to_cv2(message, desired_encoding="rgb8"))
        except Exception as error:
            self._warn_once("image_decode", f"could not decode camera image: {error}")
            return
        self._rr.log("perception/camera/color", self._rr.Image(image))

    def _sequence_callback(self, message: Any) -> None:
        stamp_ns = self._stamp_ns(message)
        with self._lock:
            if stamp_ns and stamp_ns == self._last_detection_stamp_ns:
                return
            self._last_detection_stamp_ns = stamp_ns
            self._set_time(stamp_ns)
            summary = {
                "status": str(message.status),
                "round_id": int(message.round_id),
                "current_target_id": int(message.current_target_id),
                "observed_count": int(message.observed_count),
                "expected_count": int(message.expected_count),
                "targets": [
                    {"id": int(target.id), "size": str(target.size_class), "state": str(target.task_state), "visible": bool(target.visible)}
                    for target in message.targets
                ],
            }
            self._rr.log("task/vision_sequence", self._rr.TextLog(json.dumps(summary, ensure_ascii=True)))

    def destroy(self) -> None:
        try:
            if self._recording_path:
                self._rr.save(self._recording_path)
            else:
                self._rr.disconnect()
        except Exception:
            pass


def main(args: list[str] | None = None) -> None:
    import rclpy

    rclpy.init(args=args)
    node = rclpy.create_node("rerun_visualizer")
    visualizer: RerunVisualizerNode | None = None
    try:
        visualizer = RerunVisualizerNode(node)
        node.get_logger().info("Rerun visualizer started (read-only)")
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    except Exception as error:
        node.get_logger().error(str(error))
        raise
    finally:
        if visualizer is not None:
            visualizer.destroy()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
