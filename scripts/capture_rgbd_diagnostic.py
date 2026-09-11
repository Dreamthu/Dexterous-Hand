#!/usr/bin/env python3
"""Read-only timestamp-paired RGB-D recorder; never publishes robot commands."""
import argparse
from collections import deque
import json
from pathlib import Path
import time

import cv2
import numpy as np
import rclpy
from cv_bridge import CvBridge
from rclpy.qos import qos_profile_sensor_data
from rosidl_runtime_py.convert import message_to_ordereddict
from sensor_msgs.msg import CameraInfo, Image


def stamp(message):
    return message.header.stamp.sec * 1000000000 + message.header.stamp.nanosec


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output', required=True, type=Path)
    parser.add_argument('--frames', type=int, default=45)
    parser.add_argument('--timeout', type=float, default=45)
    parser.add_argument('--max-delta-ms', type=float, default=33)
    args = parser.parse_args()
    if not 30 <= args.frames <= 60:
        parser.error('--frames must be between 30 and 60')
    args.output.mkdir(parents=True, exist_ok=False)
    rclpy.init()
    node = rclpy.create_node('rgbd_diagnostic_recorder')
    bridge = CvBridge()
    queues = {'color': deque(maxlen=90), 'depth': deque(maxlen=90)}
    received = {'color': 0, 'depth': 0}
    info = {}
    records = []
    seen = {'color': set(), 'depth': set()}

    def receive(kind, message):
        received[kind] += 1
        if stamp(message) in seen[kind]:
            return
        seen[kind].add(stamp(message))
        queues[kind].append(message)

    def pair():
        # Wait for both streams to pass the color timestamp before choosing
        # the closest depth. Every message can be used in at most one pair.
        while queues['color'] and queues['depth'] and len(records) < args.frames:
            color = queues['color'][0]
            if stamp(queues['depth'][-1]) < stamp(color):
                return
            depth = min(queues['depth'], key=lambda m: abs(stamp(m) - stamp(color)))
            delta = abs(stamp(depth) - stamp(color)) / 1e6
            queues['color'].popleft()
            if delta > args.max_delta_ms:
                continue
            while queues['depth']:
                removed = queues['depth'].popleft()
                if removed is depth:
                    break
            index = len(records)
            prefix = f'{index:03d}'
            rgb = bridge.imgmsg_to_cv2(color, desired_encoding='bgr8')
            raw_depth = bridge.imgmsg_to_cv2(depth, desired_encoding='passthrough')
            if not cv2.imwrite(str(args.output / (prefix + '_color.png')), rgb):
                raise RuntimeError('Failed to save RGB')
            np.save(args.output / (prefix + '_depth.npy'), raw_depth)
            if raw_depth.dtype == np.uint16:
                if not cv2.imwrite(str(args.output / (prefix + '_depth.png')), raw_depth):
                    raise RuntimeError('Failed to save depth PNG')
            records.append({
                'index': index, 'color_stamp_ns': stamp(color),
                'depth_stamp_ns': stamp(depth), 'delta_ms': delta,
                'color_frame_id': color.header.frame_id,
                'depth_frame_id': depth.header.frame_id,
                'color_shape': list(rgb.shape), 'depth_shape': list(raw_depth.shape),
                'depth_encoding': depth.encoding,
                'color_file': prefix + '_color.png', 'depth_file': prefix + '_depth.npy',
                'camera_info': dict(info),
            })
            if len(records) % 10 == 0:
                print(f'Captured {len(records)}/{args.frames}; delta={delta:.2f} ms', flush=True)

    subscriptions = []
    for kind in queues:
        subscriptions.append(node.create_subscription(
            Image, f'/camera/{kind}/image_raw',
            lambda m, k=kind: receive(k, m), qos_profile_sensor_data))
        subscriptions.append(node.create_subscription(
            CameraInfo, f'/camera/{kind}/camera_info',
            lambda m, k=kind: info.__setitem__(k, message_to_ordereddict(m)),
            qos_profile_sensor_data))
    start = time.monotonic()
    error = None
    try:
        while len(records) < args.frames and time.monotonic() - start < args.timeout:
            rclpy.spin_once(node, timeout_sec=0.05)
            pair()
    except Exception as exception:
        error = repr(exception)
        raise
    finally:
        result = {'requested_frames': args.frames, 'paired_frames': len(records),
                  'received': received, 'max_delta_ms': args.max_delta_ms,
                  'elapsed_seconds': time.monotonic() - start,
                  'pairing': 'nearest timestamp, one-to-one; not hardware frame sync',
                  'camera_info_latest': info, 'frames': records, 'error': error,
                  'nodes': node.get_node_names_and_namespaces(),
                  'publishers': {k: node.count_publishers(f'/camera/{k}/image_raw') for k in queues}}
        (args.output / 'capture.json').write_text(json.dumps(result, indent=2))
        print(json.dumps({k: v for k, v in result.items() if k not in ('camera_info_latest', 'frames')}, indent=2), flush=True)
        node.destroy_node()
        rclpy.shutdown()
    return 0 if len(records) >= 30 else 2


if __name__ == '__main__':
    raise SystemExit(main())
