"""Exercise the real controller against isolated fake ROS services; no hardware."""

import math
import os
import queue
import subprocess
import sys
import tempfile
import threading
import time
import unittest

import rclpy
from geometry_msgs.msg import PointStamped, PoseStamped, PoseArray, Pose
from sensor_msgs.msg import JointState
from std_msgs.msg import UInt8MultiArray
from lbot_arm_interfaces.srv import ForwardKinematics, InverseKinematics, MoveJ, MoveJP, MoveL, SetEmergency


CONTROLLER = sys.argv.pop(1)


class PregraspPipelineTest(unittest.TestCase):
    def check_pipeline(self, case, two_waypoints=False, fail_at=0, stop_rejected=False):
        waypoint_mode = case.startswith('waypoint_')
        place_mode = case.startswith('place_') or waypoint_mode
        grasp_mode = case.startswith('grasp_') or case == 'vertical_failure' or place_mode
        preview = case in ('grasp_preview', 'place_preview', 'place_independent_preview', 'waypoint_preview')
        independent = case.startswith('place_independent')
        place_reference = [-0.14, 1.5707963267948966, 0.0, -2.2] if independent else None
        rclpy.init()
        node = rclpy.create_node('pregrasp_pipeline_fake')
        namespace = f'/pregrasp_test_{os.getpid()}_{time.monotonic_ns()}'
        prefix = namespace + '/left_arm/'
        events = []
        joints = [0.0] * 7
        actual_pose = [0.4, 0.2, -0.24, 1.5, -0.07, -1.56]
        waypoint = [2.648, 2.432, -2.715, -1.078, 0.946, 0.039, 0.210]
        waypoint_pose = [0.422, 0.420, -0.227, 1.641, 0.190, -1.528]
        second_waypoint = [2.681, 3.158, -2.315, -1.074, 1.293, -0.427, 0.174]
        transfer_route = [waypoint, second_waypoint] if two_waypoints else [waypoint]
        measured_waypoint_joints = []
        ik_poses = {}
        def joint_key(values):
            # MoveJ / IK joint arrays use float32 on the ROS wire.
            return tuple(round(float(value), 5) for value in values)
        pending_pose = None
        pose_due = None
        vertical_started = False
        stale_stamp = node.get_clock().now().to_msg()
        output = []
        lines = queue.Queue()
        state_pub = node.create_publisher(JointState, prefix + 'joint_states', 10)
        pose_pub = node.create_publisher(PoseStamped, prefix + 'pose_states', 10)
        target_pub = node.create_publisher(PointStamped, namespace + '/large', 1)
        slots_pub = node.create_publisher(PoseArray, namespace + '/slots', 10)
        hand_sub = node.create_subscription(UInt8MultiArray, namespace + '/left_hand/set_l6_joint',
            lambda msg: events.append(('hand', list(msg.data))), 10)

        def publish_target(x):
            target = PointStamped()
            target.header.frame_id = 'base_link'
            target.header.stamp = node.get_clock().now().to_msg()
            target.point.x, target.point.y, target.point.z = x, 0.26, -0.51
            target_pub.publish(target)

        def publish_slots(changed=False):
            msg = PoseArray()
            msg.header.frame_id = 'base_link'
            msg.header.stamp = node.get_clock().now().to_msg()
            # The middle array entry is farthest. Never infer distance from index.
            positions = ((0.25, -0.9), (0.65, -0.4), (0.45, -0.3)) if waypoint_mode else (
                (0.25, -0.2), (0.65, -0.4), (0.45, -0.3))
            for x, y in positions:
                pose = Pose()
                pose.position.x = x if not changed else 0.1
                pose.position.y = y if not changed else 0.1
                pose.position.z = -0.55
                pose.orientation.w = 1.0
                msg.poses.append(pose)
            slots_pub.publish(msg)

        def state_tick():
            nonlocal pending_pose
            if pending_pose is not None and time.monotonic() >= pose_due:
                actual_pose[:] = pending_pose
                pending_pose = None
                events.append(('vertical_arrived', None))
            state = JointState()
            state.position = joints
            state_pub.publish(state)
            if vertical_started and case == 'grasp_missing_feedback':
                return
            pose = PoseStamped()
            pose.header.frame_id = 'base_link'
            pose.header.stamp = node.get_clock().now().to_msg()
            pose.pose.position.x, pose.pose.position.y, pose.pose.position.z = actual_pose[:3]
            roll, pitch, yaw = actual_pose[3:]
            cr, sr = math.cos(roll / 2), math.sin(roll / 2)
            cp, sp = math.cos(pitch / 2), math.sin(pitch / 2)
            cy, sy = math.cos(yaw / 2), math.sin(yaw / 2)
            q = pose.pose.orientation
            q.x = sr * cp * cy - cr * sp * sy
            q.y = cr * sp * cy + sr * cp * sy
            q.z = cr * cp * sy - sr * sp * cy
            q.w = cr * cp * cy + sr * sp * sy
            if case == 'grasp_delayed_feedback':
                # q and -q describe the same orientation.
                q.x, q.y, q.z, q.w = -q.x, -q.y, -q.z, -q.w
            if vertical_started:
                if case == 'grasp_stale_feedback':
                    pose.header.stamp = stale_stamp
                elif case == 'grasp_wrong_frame':
                    pose.header.frame_id = 'camera_link'
                elif case == 'grasp_wrong_orientation':
                    q.x, q.y, q.z, q.w = 0.0, 0.0, 0.0, 1.0
            pose_pub.publish(pose)

        def movej(request, response):
            events.append(('MoveJ', list(request.joints)))
            waypoint_number = next((i for i, q in enumerate(transfer_route)
                                    if waypoint_mode and joint_key(request.joints) == joint_key(q)), None)
            fail_here = waypoint_number == fail_at and waypoint_number is not None
            if not (fail_here and case in ('waypoint_not_reached', 'waypoint_rejected')):
                joints[:] = request.joints
                if waypoint_number is not None:
                    actual_pose[:] = waypoint_pose if waypoint_number == 0 else [0.39, -0.12, -0.14, 1.72, 0.3, -2.4]
                    if two_waypoints:
                        # Simulate a small settled joint error: the final IK must
                        # use actual feedback, not the second YAML target array.
                        joints[0] += 0.001
                    measured_waypoint_joints.append(joints.copy())
            # A later visual result must never replace the saved target.
            publish_target(0.9)
            if place_mode:
                publish_slots(changed=True)
            response.success = not (fail_here and case == 'waypoint_rejected')
            return response

        def fk(request, response):
            events.append(('FK', list(request.joints)))
            pose = ik_poses.get(joint_key(request.joints), [0.4, 0.2, -0.24, 1.5, -0.07, -1.56]).copy()
            if case == 'waypoint_bad_fk' and joint_key(request.joints) in ik_poses:
                pose[0] += 0.02  # Solver success with an inaccurate endpoint must be rejected.
            response.position.x, response.position.y, response.position.z = pose[:3]
            response.euler.x, response.euler.y, response.euler.z = pose[3:]
            if case == 'waypoint_changed_state' and joint_key(request.joints) in ik_poses:
                joints[0] += 0.1
            response.success = True
            return response

        def ik(request, response):
            pose = [request.position.x, request.position.y, request.position.z,
                    request.euler.x, request.euler.y, request.euler.z]
            events.append(('IK', (list(request.joints), pose)))
            response.joints = request.joints
            response.success = waypoint_mode and case != 'waypoint_no_solution'
            if case == 'waypoint_fallback' and sum(e[0] == 'IK' for e in events) == 1:
                response.success = False
            if response.success:
                response.joints = [0.1, 1.0, -0.5, -0.3, 0.0, 0.0, 0.0]
                if case == 'waypoint_out_of_limits':
                    response.joints[1] = 3.201
                ik_poses[joint_key(response.joints)] = pose
            return response

        def movel(request, response):
            nonlocal pending_pose, pose_due, vertical_started
            events.append(('MoveL', [request.position.x, request.position.y, request.position.z,
                                    request.euler.x, request.euler.y, request.euler.z]))
            self.assertTrue(request.block)
            response.success = case != 'vertical_failure'
            is_lift = sum(event[0] == 'MoveL' for event in events) == 2
            if is_lift and case == 'place_lift_failure':
                response.success = False
            vertical_started = True
            if (response.success and case != 'grasp_vertical_not_reached' and
                    not (is_lift and case == 'place_lift_not_reached')):
                if case == 'grasp_delayed_feedback':
                    pending_pose = events[-1][1].copy()
                    pose_due = time.monotonic() + 0.15
                else:
                    actual_pose[:] = events[-1][1]
                    if case == 'place_measured_pose' or waypoint_mode:
                        actual_pose[2] += 0.0002
                        actual_pose[3] += 0.002
                        actual_pose[5] += 0.003
                        events.append(('measured_lift' if is_lift else 'measured_descent', actual_pose.copy()))
            return response

        def movep(request, response):
            events.append(('MoveJP', [request.position.x, request.position.y, request.position.z,
                                     request.euler.x, request.euler.y, request.euler.z]))
            self.assertTrue(request.block)
            response.success = case not in ('move_pose_failure', 'grasp_move_pose_failure')
            is_transfer = sum(event[0] == 'MoveJP' for event in events) == 2
            if is_transfer and case in ('place_transfer_failure', 'place_independent_rejected', 'waypoint_transfer_rejected'):
                response.success = False
            if (response.success and case != 'grasp_approach_not_reached' and
                    not (is_transfer and case in ('place_transfer_not_reached', 'waypoint_transfer_not_reached'))):
                actual_pose[:] = events[-1][1]
            return response

        def stop(request, response):
            events.append(('stop', request.emergency))
            response.success = not stop_rejected
            return response

        services = [node.create_service(kind, prefix + name, callback) for kind, name, callback in [
            (MoveJ, 'move_joint', movej), (MoveL, 'move_linear', movel),
            (MoveJP, 'move_pose', movep),
            (ForwardKinematics, 'forward_kinematics', fk),
            (InverseKinematics, 'inverse_kinematics', ik),
            (SetEmergency, 'set_emergency_stop', stop),
        ]]
        timer = node.create_timer(0.01, state_tick)
        palm_mode = case == 'palm_success' or grasp_mode
        reference = [-0.36, 0.0, 0.0, 1.5707963267948966] if palm_mode else [-0.36, 1.08, 0.12, -1.18]
        config = f'''/**:
  ros__parameters:
    execute_task: true
    task_mode: {'validate_pregrasp' if preview else 'pregrasp'}
    robot_namespace: {namespace}
    large_target_topic: {namespace}/large
    sequence_topic: {namespace}/sequence
    slots_topic: {namespace}/slots
    event_service: {namespace}/set_state
    table_route_calibrated: true
    tool_calibrated: true
    vision_calibrated: true
    approach_reference_z_rpy: {reference}
    approach_target_is_tcp: {str(palm_mode).lower()}
    approach_grasp_enabled: {str(grasp_mode).lower()}
    approach_grasp_z_m: -0.380
    approach_place_enabled: {str(place_mode).lower()}
    approach_place_lift_m: 0.12{chr(10) + '    approach_place_reference_z_rpy: ' + str(place_reference) if independent else ''}
    approach_hand_release: [240, 30, 180, 180, 180, 180]
    approach_grasp_z_frame: {'palm' if case == 'grasp_legacy_palm' else 'arm_tip'}
    pose_arrival_timeout_ms: 700
    pose_tolerance_m: 0.001
    pose_tolerance_rad: 0.02
    pose_stable_samples: 3
    approach_hand_ready: [240, 30, 180, 180, 180, 180]
    approach_hand_close: [0, 30, 0, 0, 0, 0]
    grip_settle_ms: 100
    tcp_offset_x_m: -0.1
    tcp_offset_y_m: 0.02
    tcp_offset_z_m: {-0.03 if palm_mode else 0.0}
    route:
      names: [natural, outside, above]
      natural: [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0]
      outside: [0.6, 0.0, 0.0, -1.5, 0.0, -1.5, 0.0]
      above: [-0.7, 0.0, 0.0, -0.8, 1.5, 0.0, 0.0]
    waypoint_settle_ms: 0
    stable_joint_samples: 2
    waypoint_timeout_ms: 2000
    service_timeout_ms: 2000
    state_timeout_ms: 1000
    vision_wait_timeout_ms: 3000
'''
        if waypoint_mode:
            configured_waypoint = waypoint if case != 'waypoint_invalid_config' else [4.0] * 7
            if two_waypoints:
                configured_second = second_waypoint.copy()
                if case == 'waypoint_invalid_config':
                    configured_second[1] = 3.201
                config += f'''    approach_place_route:
      names: [transfer_1, transfer_2]
      transfer_1: {waypoint}
      transfer_2: {configured_second}
'''
            else:
                config += f'    approach_place_waypoint: {configured_waypoint}\n'
            if case == 'waypoint_conflicting_config':
                config += f'    approach_place_waypoint: {waypoint}\n'
            config += '''    approach_place_slot_selection: max_x
    approach_place_release_enabled: false
    approach_place_max_orientation_change_rad: 0.0
    approach_place_planning_timeout_ms: 2000
    left_joint_min: [-2.91, -0.07, -2.72, -2.05, -2.69, -1.59, -1.59]
    joint_limit_margin_rad: 0.0
'''
            if case == 'waypoint_fallback':
                config = config.replace('approach_place_max_orientation_change_rad: 0.0',
                                        'approach_place_max_orientation_change_rad: 0.35')
            # A stale independent Z must not override the frozen actual lift Z.
            config += '    approach_place_reference_z_rpy: [-0.14, 1.570796, 0.0, -2.2]\n'
        process = None
        reader = None
        try:
            with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml') as params:
                params.write(config)
                params.flush()
                process = subprocess.Popen(
                    [CONTROLLER, '--ros-args', '--params-file', params.name],
                    stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True,
                    env={**os.environ, 'RCUTILS_LOGGING_BUFFERED_STREAM': '0'})

                def read_output():
                    for line in process.stdout:
                        output.append(line)
                        lines.put(line)

                reader = threading.Thread(target=read_output)
                reader.start()
                waiting = False
                sent = False
                slots_waiting = False
                slots_sent = False
                deadline = time.monotonic() + 15
                while process.poll() is None and time.monotonic() < deadline:
                    rclpy.spin_once(node, timeout_sec=0.01)
                    while not lines.empty():
                        line = lines.get_nowait()
                        waiting |= 'waiting for one current LARGE nut position' in line
                        slots_waiting |= 'waiting for one current three-slot observation' in line
                    if waiting and not sent and case != 'no_target' and target_pub.get_subscription_count():
                        events.append(('capture', None))
                        publish_target(0.48)
                        sent = True
                    if (slots_waiting and not slots_sent and case != 'place_no_slots' and
                            slots_pub.get_subscription_count()):
                        publish_slots()
                        slots_sent = True
                self.assertIsNotNone(process.poll(), ''.join(output))
                reader.join(timeout=2)
                # Verify terminal cleanup separately from the grasp/place stage trace.
                # It must happen even when arrival validation failed or slot release is disabled.
                cleanup_open = ('hand', [255, 40, 255, 255, 255, 255])
                if any(event[0] == 'MoveJ' for event in events):
                    if process.returncode == 0:
                        self.assertEqual(events[-3:], [cleanup_open] * 3, ''.join(output))
                        del events[-3:]
                    else:
                        self.assertEqual(events[-4:], [cleanup_open] * 3 + [('stop', True)], ''.join(output))
                        self.assertEqual(sum(event[0] == 'stop' for event in events), 1)
                        del events[-4:-1]
                        if stop_rejected:
                            self.assertIn('task cleanup emergency stop failed', ''.join(output))
                else:
                    self.assertNotIn(cleanup_open, events)
                names = [event[0] for event in events]
                if case in ('waypoint_invalid_config', 'waypoint_conflicting_config'):
                    self.assertEqual(events, [], ''.join(output))
                    self.assertNotEqual(process.returncode, 0)
                elif waypoint_mode and not preview:
                    self.assertEqual(names[:4], ['capture', 'MoveJ', 'MoveJ', 'FK'], ''.join(output))
                    no_ik = case in ('waypoint_rejected', 'waypoint_not_reached')
                    expected_route = transfer_route[:fail_at+1] if no_ik else transfer_route
                    self.assertEqual(names.count('MoveJ'), 2 + len(expected_route), ''.join(output))
                    post_lift_joints = [e[1] for e in events if e[0] == 'MoveJ'][2:]
                    for actual_joints, expected_joints in zip(post_lift_joints, expected_route):
                        for actual, expected in zip(actual_joints, expected_joints):
                            self.assertAlmostEqual(actual, expected, places=6)
                    waypoint_index = max(i for i, e in enumerate(events) if e[0] == 'MoveJ')
                    self.assertEqual(names.count('MoveL'), 2)
                    lift_index = max(i for i, e in enumerate(events) if e[0] == 'MoveL')
                    first_waypoint_index = [i for i, e in enumerate(events) if e[0] == 'MoveJ'][2]
                    self.assertLess(lift_index, first_waypoint_index)
                    self.assertEqual(sum(e == ('hand', [240, 30, 180, 180, 180, 180]) for e in events), 3)
                    self.assertEqual(sum(e == ('hand', [0, 30, 0, 0, 0, 0]) for e in events), 3)
                    # The transfer stage keeps the grasp; cleanup was checked above.
                    self.assertFalse(any(e[0] == 'hand' for e in events[first_waypoint_index:]))
                    if no_ik:
                        self.assertNotIn('IK', names)
                    else:
                        self.assertEqual(names.count('IK'), 2 if case == 'waypoint_fallback' else 1)
                        self.assertLess(waypoint_index, names.index('IK'))
                        lifted_actual = next(e[1] for e in events if e[0] == 'measured_lift')
                        for _, (seed, target) in (e for e in events if e[0] == 'IK'):
                            for actual, expected in zip(seed, measured_waypoint_joints[-1]):
                                self.assertAlmostEqual(actual, expected, places=6)
                            self.assertAlmostEqual(target[2], lifted_actual[2], places=9)
                            self.assertNotAlmostEqual(target[2], waypoint_pose[2], places=3)
                        first_ik_target = next(e[1][1] for e in events if e[0] == 'IK')
                        for actual, expected in zip(first_ik_target[3:], lifted_actual[3:]):
                            self.assertAlmostEqual(actual, expected, places=9)
                    transfer_sent = case not in ('waypoint_rejected', 'waypoint_not_reached',
                                                 'waypoint_changed_state')
                    self.assertEqual(names.count('MoveJP'), 2 if transfer_sent else 1, ''.join(output))
                    if transfer_sent:
                        target = [e[1] for e in events if e[0] == 'MoveJP'][-1]
                        selected_ik = [e[1][1] for e in events if e[0] == 'IK'][-1]
                        self.assertEqual(target, selected_ik)
                        r, p, y = target[3:]
                        ox = math.cos(y)*math.cos(p)*(-.1)+(math.cos(y)*math.sin(p)*math.sin(r)-math.sin(y)*math.cos(r))*.02+(math.cos(y)*math.sin(p)*math.cos(r)+math.sin(y)*math.sin(r))*(-.03)
                        oy = math.sin(y)*math.cos(p)*(-.1)+(math.sin(y)*math.sin(p)*math.sin(r)+math.cos(y)*math.cos(r))*.02+(math.sin(y)*math.sin(p)*math.cos(r)-math.cos(y)*math.sin(r))*(-.03)
                        self.assertAlmostEqual(target[0]+ox, .65, places=9)
                        self.assertAlmostEqual(target[1]+oy, -.4, places=9)
                    if case in ('waypoint_success', 'waypoint_fallback', 'waypoint_no_solution',
                                'waypoint_bad_fk', 'waypoint_out_of_limits'):
                        self.assertEqual(process.returncode, 0, ''.join(output))
                        self.assertNotIn('stop', names)
                        self.assertIn('task cleanup will open hand', ''.join(output))
                        self.assertIn('saved max_x slot_2', ''.join(output))
                        if two_waypoints:
                            self.assertIn('reached LARGE post-lift transfer waypoint 1/2 (transfer_1)', ''.join(output))
                            self.assertIn('reached LARGE post-lift transfer waypoint 2/2 (transfer_2)', ''.join(output))
                    else:
                        self.assertNotEqual(process.returncode, 0, ''.join(output))
                        self.assertEqual(events[-1], ('stop', True))
                elif case == 'no_target':
                    self.assertEqual(events, [])
                    self.assertNotEqual(process.returncode, 0)
                elif case == 'place_no_slots':
                    self.assertEqual(names, ['capture'], ''.join(output))
                    self.assertNotEqual(process.returncode, 0)
                elif preview:
                    self.assertEqual(names, ['capture', 'FK'], ''.join(output))
                    self.assertEqual(process.returncode, 0, ''.join(output))
                else:
                    self.assertEqual(names[:4], ['capture', 'MoveJ', 'MoveJ', 'FK'], ''.join(output))
                    self.assertEqual(events[3][1], joints)
                    self.assertNotIn('IK', names)
                    transfer_sent = place_mode and case not in ('place_lift_failure', 'place_lift_not_reached')
                    self.assertEqual(names.count('MoveJP'), 2 if transfer_sent else 1, ''.join(output))
                    if case in ('move_pose_failure', 'grasp_move_pose_failure', 'grasp_approach_not_reached'):
                        self.assertNotIn('hand', names)
                        self.assertNotIn('MoveL', names)
                        self.assertEqual(events[-1], ('stop', True))
                        self.assertNotEqual(process.returncode, 0)
                    else:
                        expected = [0.50, 0.36, -0.33, 0.0, 0.0, reference[3]] if palm_mode else [0.48, 0.26, *reference]
                        approach_event = next(event for event in events if event[0] == 'MoveJP')
                        for actual, goal in zip(approach_event[1], expected):
                            self.assertAlmostEqual(actual, goal, places=9)
                        if grasp_mode:
                            self.assertEqual(names.count('MoveL'), 2 if place_mode else 1)
                            linear_event = next(event for event in events if event[0] == 'MoveL')
                            lowered = expected.copy()
                            lowered[2] = -0.35 if case == 'grasp_legacy_palm' else -0.380
                            for actual, goal in zip(linear_event[1], lowered):
                                self.assertAlmostEqual(actual, goal, places=9)
                            # Adapter repeats each hand command three times; verify stage order.
                            stages = []
                            for event in events:
                                if event[0] in ('MoveJP', 'hand', 'MoveL', 'stop'):
                                    if not stages or event != stages[-1]:
                                        stages.append(event)
                            self.assertEqual(stages[0][0], 'MoveJP')
                            self.assertEqual(stages[1], ('hand', [240, 30, 180, 180, 180, 180]))
                            self.assertEqual(stages[2][0], 'MoveL')
                            if case in ('vertical_failure', 'grasp_vertical_not_reached',
                                        'grasp_missing_feedback', 'grasp_stale_feedback',
                                        'grasp_wrong_frame', 'grasp_wrong_orientation'):
                                self.assertEqual(stages[3:], [('stop', True)])
                                self.assertNotEqual(process.returncode, 0)
                                if case != 'vertical_failure':
                                    self.assertIn('pose arrival timeout', ''.join(output))
                            elif place_mode:
                                self.assertEqual(stages[3], ('hand', [0, 30, 0, 0, 0, 0]))
                                self.assertEqual(stages[4][0], 'MoveL')
                                lift = stages[4][1]
                                descent_actual = (next(e[1] for e in events if e[0] == 'measured_descent')
                                                  if case == 'place_measured_pose' else lowered)
                                lift_expected = descent_actual.copy()
                                lift_expected[2] += 0.12
                                for actual, goal in zip(lift, lift_expected):
                                    self.assertAlmostEqual(actual, goal, places=9)
                                if transfer_sent:
                                    self.assertEqual(stages[5][0], 'MoveJP')
                                    transferred = stages[5][1]
                                    lifted_actual = (next(e[1] for e in events if e[0] == 'measured_lift')
                                                     if case == 'place_measured_pose' else lift)
                                    target_reference = place_reference if independent else lifted_actual[2:]
                                    for actual, goal in zip(transferred[2:], target_reference):
                                        self.assertAlmostEqual(actual, goal, places=9)
                                    r, p, y = target_reference[1:]
                                    cr, sr, cp, sp, cy, sy = (math.cos(r), math.sin(r), math.cos(p),
                                                            math.sin(p), math.cos(y), math.sin(y))
                                    ox = cy*cp*(-0.1) + (cy*sp*sr-sy*cr)*0.02 + (cy*sp*cr+sy*sr)*(-0.03)
                                    oy = sy*cp*(-0.1) + (sy*sp*sr+cy*cr)*0.02 + (sy*sp*cr-cy*sr)*(-0.03)
                                    self.assertAlmostEqual(transferred[0] + ox, 0.65, places=9)
                                    self.assertAlmostEqual(transferred[1] + oy, -0.4, places=9)
                                failed = case in ('place_lift_failure', 'place_lift_not_reached',
                                                  'place_transfer_failure', 'place_transfer_not_reached',
                                                  'place_independent_rejected')
                                if failed:
                                    self.assertEqual(stages[-1], ('stop', True))
                                    self.assertNotEqual(process.returncode, 0)
                                    self.assertEqual(sum(e == ('hand', [240, 30, 180, 180, 180, 180])
                                                         for e in stages), 1)
                                else:
                                    self.assertEqual(stages[6:], [('hand', [240, 30, 180, 180, 180, 180])])
                                    self.assertEqual(process.returncode, 0, ''.join(output))
                                    self.assertIn('saved farthest slot_2', ''.join(output))
                                    self.assertEqual(''.join(output).count('pose arrival confirmed'), 5)
                            else:
                                self.assertEqual(stages[3:], [('hand', [0, 30, 0, 0, 0, 0])])
                                self.assertEqual(process.returncode, 0, ''.join(output))
                                self.assertEqual(''.join(output).count('pose arrival confirmed'), 2)
                                if case == 'grasp_delayed_feedback':
                                    arrived = names.index('vertical_arrived')
                                    closed = events.index(('hand', [0, 30, 0, 0, 0, 0]))
                                    self.assertLess(arrived, closed)
                        else:
                            self.assertNotIn('MoveL', names)
                            self.assertNotIn('hand', names)
                            self.assertNotIn('stop', names)
                            self.assertEqual(process.returncode, 0, ''.join(output))
        finally:
            if process is not None:
                if process.poll() is None:
                    process.kill()
                process.wait()
                if reader is not None:
                    reader.join(timeout=2)
                process.stdout.close()
            node.destroy_node()
            rclpy.shutdown()

    def test_capture_then_enter_then_move_pose_with_saved_target(self):
        self.check_pipeline('success')

    def test_no_target_never_starts_enter(self):
        self.check_pipeline('no_target')

    def test_palm_target_converts_xyz_and_preserves_rpy(self):
        self.check_pipeline('palm_success')

    def test_move_pose_failure_after_enter_stops_without_retry(self):
        self.check_pipeline('move_pose_failure')

    def test_approach_then_ready_then_vertical_then_close(self):
        self.check_pipeline('grasp_success')

    def test_failed_approach_opens_hand_before_stop_without_vertical_motion(self):
        self.check_pipeline('grasp_move_pose_failure')

    def test_rejected_emergency_stop_still_follows_open_command(self):
        self.check_pipeline('vertical_failure', stop_rejected=True)

    def test_failed_vertical_move_never_closes_hand(self):
        self.check_pipeline('vertical_failure')

    def test_grasp_preview_sends_no_hand_or_motion_commands(self):
        self.check_pipeline('grasp_preview')

    def test_move_pose_success_without_arrival_never_moves_hand(self):
        self.check_pipeline('grasp_approach_not_reached')

    def test_move_linear_success_without_arrival_never_closes_hand(self):
        self.check_pipeline('grasp_vertical_not_reached')

    def test_delayed_feedback_gates_closing_and_accepts_equivalent_quaternion(self):
        self.check_pipeline('grasp_delayed_feedback')

    def test_unusable_vertical_feedback_never_closes_hand(self):
        for case in ('grasp_missing_feedback', 'grasp_stale_feedback',
                     'grasp_wrong_frame', 'grasp_wrong_orientation'):
            with self.subTest(case=case):
                self.check_pipeline(case)

    def test_explicit_palm_height_keeps_tcp_conversion(self):
        self.check_pipeline('grasp_legacy_palm')

    def test_place_at_saved_farthest_slot_after_12cm_lift(self):
        self.check_pipeline('place_success')

    def test_lift_and_transfer_preserve_measured_pose(self):
        self.check_pipeline('place_measured_pose')

    def test_missing_slot_never_starts_arm(self):
        self.check_pipeline('place_no_slots')

    def test_placement_preview_sends_no_motion_or_hand_commands(self):
        self.check_pipeline('place_preview')

    def test_lift_or_transfer_failure_never_releases(self):
        for case in ('place_lift_failure', 'place_lift_not_reached',
                     'place_transfer_failure', 'place_transfer_not_reached'):
            with self.subTest(case=case):
                self.check_pipeline(case)

    def test_independent_placement_pose_preserves_grasp_and_lift(self):
        self.check_pipeline('place_independent_pose')

    def test_independent_placement_preview_sends_no_commands(self):
        self.check_pipeline('place_independent_preview')

    def test_rejected_independent_placement_never_releases(self):
        self.check_pipeline('place_independent_rejected')

    def test_waypoint_preserves_actual_lift_reference_and_cached_max_x_without_release(self):
        self.check_pipeline('waypoint_success')

    def test_waypoint_nearby_orientation_fallback_rotates_tcp_without_changing_z(self):
        self.check_pipeline('waypoint_fallback')

    def test_waypoint_motion_failures_stop_and_ik_does_not_veto_move_pose(self):
        for case in ('waypoint_rejected', 'waypoint_not_reached', 'waypoint_no_solution',
                     'waypoint_bad_fk', 'waypoint_out_of_limits', 'waypoint_changed_state',
                     'waypoint_transfer_rejected', 'waypoint_transfer_not_reached'):
            with self.subTest(case=case):
                self.check_pipeline(case)

    def test_waypoint_preview_does_not_send_motion_ik_or_hand(self):
        self.check_pipeline('waypoint_preview')

    def test_invalid_waypoint_rejected_before_capture_or_motion(self):
        self.check_pipeline('waypoint_invalid_config')

    def test_two_waypoints_run_in_order_and_plan_from_actual_second_point_with_frozen_lift(self):
        self.check_pipeline('waypoint_success', two_waypoints=True)

    def test_two_waypoints_preserve_orientation_fallback_and_move_pose_authority(self):
        for case in ('waypoint_fallback', 'waypoint_no_solution'):
            with self.subTest(case=case):
                self.check_pipeline(case, two_waypoints=True)

    def test_either_waypoint_failure_prevents_later_moves_and_release(self):
        for fail_at in (0, 1):
            for case in ('waypoint_rejected', 'waypoint_not_reached'):
                with self.subTest(case=case, point=fail_at + 1):
                    self.check_pipeline(case, two_waypoints=True, fail_at=fail_at)

    def test_invalid_second_waypoint_is_rejected_before_first_motion(self):
        self.check_pipeline('waypoint_invalid_config', two_waypoints=True)

    def test_conflicting_legacy_and_route_settings_fail_before_motion(self):
        self.check_pipeline('waypoint_conflicting_config', two_waypoints=True)

    def test_two_waypoint_preview_sends_no_motion_or_hand_commands(self):
        self.check_pipeline('waypoint_preview', two_waypoints=True)


if __name__ == '__main__':
    unittest.main()
