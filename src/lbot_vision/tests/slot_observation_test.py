"""Real detector, synthetic images, isolated topics: slots need no nut sequence."""

import os
import subprocess
import sys
import tempfile
import time
import unittest

import cv2
import numpy as np
import rclpy
import yaml
from geometry_msgs.msg import PoseArray, PointStamped, TransformStamped
from sensor_msgs.msg import Image, CameraInfo
from lbot_vision.msg import NutSequenceState
from tf2_ros import StaticTransformBroadcaster


DETECTOR, CONFIG = sys.argv[1:3]
del sys.argv[1:3]


class SlotObservationTest(unittest.TestCase):
    def test_one_nut_still_publishes_three_localized_slots(self):
        self.run_scene()

    def test_rotated_camera_uses_robot_x(self):
        self.run_scene(rotated=True)

    def test_missing_tf_does_not_publish_camera_axis_slots(self):
        self.run_scene(missing_tf=True)

    def test_perspective_large_stream_and_sequence_agree(self):
        self.run_scene(perspective=True)

    def test_slot_height_uses_floor_depth(self):
        self.run_scene(floor_depth=1200)

    def test_near_border_large_stream_and_sequence_agree(self):
        self.run_scene(near_border=True)

    def run_scene(self, rotated=False, missing_tf=False, perspective=False, floor_depth=1000,
                  near_border=False):
        rclpy.init()
        node = rclpy.create_node('slot_observation_test')
        prefix = f'/slot_observation_test_{os.getpid()}_{time.monotonic_ns()}'
        with open(CONFIG) as source:
            params = yaml.safe_load(source)['nut_detector_node']['ros__parameters']
        topic_keys = ('color_topic', 'depth_topic', 'camera_info_topic', 'detection_topic',
                      'large_target_topic', 'slot_topic', 'debug_image_topic',
                      'sequence_topic', 'sequence_event_service')
        params.update({name: prefix + '/' + name for name in topic_keys})
        params.update(use_tf=rotated or missing_tf, target_frame='base_link', publish_rate_hz=20.0,
                      debug_black_frame=False)
        # This fixture draws pure BGR blue; field calibration is for the physical basket.
        params.update(blue_h_min=90, blue_h_max=140, blue_s_min=70, blue_s_max=255,
                      blue_v_min=35, blue_v_max=255)
        camera_frame = prefix[1:] + '_camera' if rotated or missing_tf else 'base_link'
        broadcaster = None
        if rotated:
            broadcaster = StaticTransformBroadcaster(node)
            transform = TransformStamped()
            transform.header.stamp = node.get_clock().now().to_msg()
            transform.header.frame_id = 'base_link'
            transform.child_frame_id = camera_frame
            transform.transform.translation.x = 0.6
            transform.transform.rotation.z = -float(np.sqrt(0.5))
            transform.transform.rotation.w = float(np.sqrt(0.5))
            broadcaster.sendTransform(transform)
        color_pub = node.create_publisher(Image, params['color_topic'], 10)
        depth_pub = node.create_publisher(Image, params['depth_topic'], 10)
        info_pub = node.create_publisher(CameraInfo, params['camera_info_topic'], 10)
        slots, sequences, large = [], [], []
        subscriptions = [
            node.create_subscription(PoseArray, params['slot_topic'], slots.append, 10),
            node.create_subscription(NutSequenceState, params['sequence_topic'], sequences.append, 10),
            node.create_subscription(PointStamped, params['large_target_topic'], large.append, 10),
        ]
        color = np.full((600, 800, 3), 240, dtype=np.uint8)
        cv2.rectangle(color, (30, 40), (229, 219), (0, 0, 0), 6)
        cv2.rectangle(color, (490, 120), (599, 419), (255, 0, 0), -1)
        angles = np.arange(6) * np.pi / 3
        # The large nut has a one-pixel visible gap to the left/top frame edges.
        nuts = (((55, 62), 20), ((175, 105), 16), ((125, 165), 12)) if near_border else (((85, 95), 20),)
        for (u, v), radius in nuts:
            polygon = np.rint(np.c_[u + radius*np.cos(angles), v + radius*np.sin(angles)]).astype(np.int32)
            cv2.fillConvexPoly(color, polygon, (65, 65, 65))
            cv2.circle(color, (u, v), radius // 3, (240, 240, 240), -1)
        if perspective:
            plane = np.full((401, 401, 3), 240, dtype=np.uint8)
            cv2.rectangle(plane, (10, 10), (390, 390), (0, 0, 0), 6)
            for (u, v), radius in zip(((150, 80), (270, 230), (140, 330)), (30, 24, 20)):
                polygon = np.rint(np.c_[u + radius*np.cos(angles), v + radius*np.sin(angles)]).astype(np.int32)
                cv2.fillConvexPoly(plane, polygon, (65, 65, 65))
                cv2.circle(plane, (u, v), radius // 3, (240, 240, 240), -1)
            homography = cv2.getPerspectiveTransform(
                np.float32([[10, 10], [390, 10], [390, 390], [10, 390]]),
                np.float32([[180, 50], [270, 50], [370, 280], [80, 280]]))
            color = cv2.warpPerspective(plane, homography, (800, 600), borderValue=(240, 240, 240))
            cv2.rectangle(color, (490, 120), (599, 419), (255, 0, 0), -1)
        depth = np.full((600, 800), 1000, dtype=np.uint16)
        if floor_depth != 1000:
            depth[130:410, 500:590] = floor_depth
        image = Image(height=600, width=800, encoding='bgr8', step=2400, data=color.tobytes())
        depth_image = Image(height=600, width=800, encoding='16UC1', step=1600, data=depth.tobytes())
        info = CameraInfo(height=600, width=800, distortion_model='plumb_bob')
        info.k = [600.0, 0.0, 400.0, 0.0, 600.0, 300.0, 0.0, 0.0, 1.0]
        info.p = [600.0, 0.0, 400.0, 0.0, 0.0, 600.0, 300.0, 0.0, 0.0, 0.0, 1.0, 0.0]
        info.r = [1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0]
        info.d = [0.0] * 5
        process = None
        try:
            with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml') as config, tempfile.TemporaryFile(mode='w+') as output:
                yaml.safe_dump({'/**': {'ros__parameters': params}}, config)
                config.flush()
                process = subprocess.Popen([DETECTOR, '--ros-args', '--params-file', config.name],
                                           stdout=output, stderr=subprocess.STDOUT)
                deadline = time.monotonic() + 10
                next_frame = 0.0
                while time.monotonic() < deadline and process.poll() is None:
                    rclpy.spin_once(node, timeout_sec=0.01)
                    if time.monotonic() >= next_frame and all(p.get_subscription_count() for p in (color_pub, depth_pub, info_pub)):
                        stamp = node.get_clock().now().to_msg()
                        for msg in (image, depth_image, info):
                            msg.header.stamp = stamp
                            msg.header.frame_id = camera_frame
                        info_pub.publish(info)
                        depth_pub.publish(depth_image)
                        color_pub.publish(image)
                        next_frame = time.monotonic() + 0.08
                    if missing_tf and sum(msg.observed_count == 1 for msg in sequences) >= 3:
                        break
                    expected = 3 if perspective or near_border else 1
                    stable = not (perspective or near_border) or any(
                        msg.observation_valid and all(t.position_valid for t in msg.targets)
                        for msg in sequences)
                    if not missing_tf and stable and any(len(msg.poses) == 3 for msg in slots) and large and any(msg.observed_count == expected for msg in sequences):
                        break
                output.seek(0)
                log = output.read()
                localized = [msg for msg in slots if len(msg.poses) == 3]
                discovery = [p.get_subscription_count() for p in (color_pub, depth_pub, info_pub)]
                if missing_tf:
                    self.assertTrue(any(msg.observed_count == 1 for msg in sequences), log)
                    self.assertFalse(localized, 'camera coordinates were used for robot-X slots')
                    self.assertFalse(large, 'camera target was published as a base target')
                    return
                self.assertTrue(localized, f'{log}\nsubscriptions={discovery}, sequences={len(sequences)}, large={len(large)}')
                self.assertTrue(large, log)
                self.assertTrue(any(msg.observed_count == expected for msg in sequences), log)
                if perspective:
                    valid = [msg for msg in sequences if msg.observation_valid and
                             all(t.position_valid for t in msg.targets)]
                    self.assertTrue(valid, log)
                    targets = valid[-1].targets
                    self.assertLess(targets[0].last_v, 100)
                    self.assertGreater(targets[2].last_v, 180)
                    self.assertLess(targets[0].last_radius_px, targets[2].last_radius_px)
                    self.assertLess(large[-1].point.y, -0.33)
                    self.assertAlmostEqual(large[-1].point.y, targets[0].position.point.y, places=3)
                elif near_border:
                    valid = [msg for msg in sequences if msg.observation_valid and
                             all(t.position_valid for t in msg.targets)]
                    self.assertTrue(valid, log)
                    target = valid[-1].targets[0]
                    self.assertAlmostEqual(target.last_u, 55, delta=1.0)
                    self.assertAlmostEqual(target.last_v, 62, delta=1.0)
                    self.assertAlmostEqual(large[-1].point.x, target.position.point.x, places=3)
                    self.assertAlmostEqual(large[-1].point.y, target.position.point.y, places=3)
                else:
                    self.assertFalse(any(msg.initialized for msg in sequences), log)
                self.assertEqual(localized[-1].header.frame_id, 'base_link')
                # This image basket is taller than it is wide. The split must
                # still follow base X, not its apparent long axis.
                positions = [p.position for p in localized[-1].poses]
                self.assertEqual(len({round(p.x, 3) for p in positions}), 3)
                self.assertEqual(len({round(p.y, 3) for p in positions}), 1)
                self.assertLess(positions[0].x, positions[1].x)
                self.assertLess(positions[1].x, positions[2].x)
                self.assertAlmostEqual(positions[1].x - positions[0].x,
                                       positions[2].x - positions[1].x)
                for pose in localized[-1].poses:
                    self.assertAlmostEqual(pose.position.z, floor_depth * 0.001)
                    self.assertGreater(pose.position.x, 0.15)
        finally:
            if process is not None:
                process.terminate()
                try:
                    process.wait(timeout=3)
                except subprocess.TimeoutExpired:
                    process.kill()
                    process.wait()
            node.destroy_node()
            rclpy.shutdown()


if __name__ == '__main__':
    unittest.main()
