"""Isolated synthetic PointCloud2/TF -> octomap -> real MoveIt; no robot clients."""
import csv
import importlib.util
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time
import unittest
import numpy as np
import yaml
import rclpy
from rclpy.qos import qos_profile_sensor_data
from sensor_msgs.msg import PointCloud2
from sensor_msgs_py.point_cloud2 import create_cloud_xyz32
from std_msgs.msg import Header
from geometry_msgs.msg import TransformStamped
from tf2_ros import StaticTransformBroadcaster
from moveit_test_geometry import Geometry

EXECUTABLE = sys.argv.pop(1)
PACKAGE = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location('moveit_config', PACKAGE/'python/moveit_config.py')
MODEL = importlib.util.module_from_spec(SPEC); SPEC.loader.exec_module(MODEL)
TASK = yaml.safe_load((PACKAGE.parents[1]/'config/control/nut_task.yaml').read_text())['/**']['ros__parameters']
WP1 = [2.648, 2.432, -2.715, -1.078, .946, .039, .210]
WP2 = [2.681, 3.158, -2.315, -1.074, 1.293, -.427, .174]


class CloudTest(unittest.TestCase):
    def preview(self, kind):
        task = dict(TASK)
        if kind == 'empty_corner_box': task['moveit_hand_collision_model'] = 'box'
        config = MODEL.parameters(task)
        geometry = Geometry(config['robot_description'], TASK['moveit_hand_envelope'])
        start = WP1 if kind == 'blocked_goal' else WP2
        goal = geometry.pose(WP2); goal[0] += .01
        config.update(preview_start_joints=start, preview_goal_pose=goal, preview_timeout_s=3.,
                      preview_capture_cloud=True, moveit_point_cloud_enabled=True,
                      moveit_point_cloud_topic='/cloud_test/points', moveit_point_cloud_min_points=100,
                      moveit_point_cloud_timeout_ms=1800, moveit_point_cloud_max_age_ms=500,
                      moveit_transfer_keep_above=True)
        if kind in ('empty_corner_box', 'empty_corner_cad', 'occupied_hand'):
            start = [.2153044,.1222248,-1.1900511,-1.6939039,1.3453884,-.7631416,-.5857553]
            end = list(start); end[0] += .005
            config.update(preview_start_joints=start, preview_goal_pose=geometry.pose(end),
                          preview_goal_joints=end, preview_cloud_filter_joints=[0.]*7)
        rclpy.init(); node = rclpy.create_node('synthetic_obstacle_camera')
        publisher = node.create_publisher(PointCloud2, '/cloud_test/points', qos_profile_sensor_data)
        broadcaster = StaticTransformBroadcaster(node)
        x, y = np.meshgrid(np.linspace(.15, .75, 31), np.linspace(-.6, .6, 41))
        points = np.column_stack([x.ravel(), y.ravel(), np.full(x.size, -.7)]).astype(np.float32)
        frame = 'base_link'
        if kind == 'transformed':
            frame = 'synthetic_camera'; points[:, 0] -= 1.
            tf = TransformStamped(); tf.header.frame_id = 'base_link'; tf.child_frame_id = frame
            tf.transform.translation.x = 1.; tf.transform.rotation.w = 1.
            broadcaster.sendTransform(tf)
        elif kind == 'missing_tf':
            frame = 'missing_camera_transform'
        elif kind == 'empty':
            points[:] = np.nan
        elif kind == 'blocked_goal':
            offsets = np.array(np.meshgrid(*[np.linspace(-.055, .055, 15)]*3)).reshape(3, -1).T
            points = (np.array(goal[:3])+offsets).astype(np.float32)
        elif kind in ('empty_corner_box', 'empty_corner_cad', 'occupied_hand'):
            # The same recorded lifted pose and the same synthetic obstacle:
            # old box's lowest empty corner vs a vertex on the true padded hull.
            center = ([.196793883,.432756574,-.446255113] if kind == 'occupied_hand'
                      else [.286991117,.463753364,-.472452309])
            offsets = np.array(np.meshgrid(*[np.linspace(-.004, .004, 6)]*3)).reshape(3,-1).T
            points = (np.array(center)+offsets).astype(np.float32)
        cloud = create_cloud_xyz32(Header(frame_id=frame), points)
        if kind == 'malformed':
            cloud.row_step = 1
        process = None
        try:
            with tempfile.TemporaryDirectory(prefix='lbot_cloud_test_') as directory:
                path = Path(directory); config['preview_output'] = str(path/'path.csv')
                (path/'params.yaml').write_text(yaml.safe_dump({'/**': {'ros__parameters': config}}))
                with (path/'output.log').open('w+') as log:
                    process = subprocess.Popen([EXECUTABLE, '--ros-args', '--params-file', str(path/'params.yaml')],
                                               stdout=log, stderr=log, env=dict(os.environ, ROS_LOG_DIR=str(path/'ros_logs')))
                    deadline = time.monotonic()+18.
                    while process.poll() is None and time.monotonic() < deadline:
                        cloud.header.stamp = (node.get_clock().now()-rclpy.duration.Duration(
                            seconds=10 if kind == 'stale' else 0)).to_msg()
                        if kind != 'missing': publisher.publish(cloud)
                        rclpy.spin_once(node, timeout_sec=.03)
                    if process.poll() is None: process.kill(); process.wait()
                    log.seek(0); output = log.read()
                rows = list(csv.reader((path/'path.csv').open())) if (path/'path.csv').exists() else []
                return process.returncode, output, rows
        finally:
            if process is not None and process.poll() is None: process.kill(); process.wait()
            node.destroy_node(); rclpy.shutdown()

    def test_cloud_transform_and_safe_trajectory(self):
        code, text, rows = self.preview('transformed')
        self.assertEqual(code, 0, text)
        self.assertIn('synthetic_camera -> base_link', text)
        self.assertIn('point-cloud snapshot ready', text)
        self.assertGreater(len(rows), 5)

    def test_occupied_goal_is_rejected_without_trajectory(self):
        code, text, rows = self.preview('blocked_goal')
        self.assertNotEqual(code, 0, text)
        self.assertIn('point-cloud snapshot ready', text)
        self.assertIn('no trajectory sent', text)
        self.assertFalse(rows)

    def test_cad_hull_removes_empty_corner_collision_but_keeps_real_hand_collision(self):
        code, text, rows = self.preview('empty_corner_box')
        self.assertNotEqual(code, 0, text)
        self.assertIn('start state rejected: collision <octomap>/left_hand_envelope', text)
        self.assertIn('contact_base=[', text)
        self.assertFalse(rows)
        code, text, rows = self.preview('empty_corner_cad')
        self.assertEqual(code, 0, text)
        self.assertGreater(len(rows), 5)
        code, text, rows = self.preview('occupied_hand')
        self.assertNotEqual(code, 0, text)
        self.assertIn('start state rejected: collision <octomap>/left_hand_envelope', text)
        self.assertFalse(rows)

    def test_missing_stale_untransformable_empty_and_malformed_cloud_fail_closed(self):
        for kind, reason in [('missing', 'no fresh PointCloud2'), ('stale', 'stale point-cloud timestamp'),
                             ('missing_tf', 'TF unavailable'), ('empty', 'insufficient obstacle points'),
                             ('malformed', 'malformed or oversized')]:
            with self.subTest(kind=kind):
                code, text, rows = self.preview(kind)
                self.assertNotEqual(code, 0, text)
                self.assertIn(reason, text)
                self.assertFalse(rows)


if __name__ == '__main__':
    unittest.main()
