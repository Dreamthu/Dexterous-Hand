"""Real task + MoveIt planner, fake left driver/vision. Never accesses real hardware."""
import copy
import importlib.util
import math
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import time
import unittest
import xml.etree.ElementTree as ET
import numpy as np
import yaml
import rclpy
from geometry_msgs.msg import PointStamped, PoseArray, Pose, PoseStamped
from sensor_msgs.msg import JointState, PointCloud2
from sensor_msgs_py.point_cloud2 import create_cloud_xyz32
from std_msgs.msg import Header
from std_msgs.msg import UInt8MultiArray
from lbot_arm_interfaces.msg import FollowJoint
from lbot_arm_interfaces.srv import MoveJ, MoveJP, MoveL, ForwardKinematics, SetEmergency

EXECUTABLE = sys.argv.pop(1)
PACKAGE = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location("moveit_config", PACKAGE / "python/moveit_config.py")
MODEL = importlib.util.module_from_spec(SPEC); SPEC.loader.exec_module(MODEL)
TASK = yaml.safe_load((PACKAGE.parents[1] / "config/control/nut_task.yaml").read_text())["/**"]["ros__parameters"]


def rotation(r, p, y):
    cr, sr, cp, sp, cy, sy = math.cos(r), math.sin(r), math.cos(p), math.sin(p), math.cos(y), math.sin(y)
    return np.array([[cy*cp, cy*sp*sr-sy*cr, cy*sp*cr+sy*sr],
                     [sy*cp, sy*sp*sr+cy*cr, sy*sp*cr-cy*sr], [-sp, cp*sr, cp*cr]])


class Pipeline(unittest.TestCase):
    def test_real_moveit_to_joint_follow_opens_at_exit_when_slot_release_disabled(self):
        self.check_pipeline(use_waypoints=True, inject_drift=True, release=False)

    def test_direct_transfer_from_lift_without_waypoints(self):
        self.check_pipeline(use_waypoints=False, inject_drift=False, release=True, point_cloud='valid')

    def test_missing_obstacle_cloud_fails_before_enter(self):
        self.check_pipeline(use_waypoints=False, inject_drift=False, release=True, point_cloud='missing')

    def test_stale_feedback_opens_before_stop_and_reports_age(self):
        self.check_pipeline(use_waypoints=False, inject_drift=False, release=True, feedback_fault='stale')

    def test_measured_limit_violation_opens_before_stop_and_identifies_joint(self):
        self.check_pipeline(use_waypoints=False, inject_drift=False, release=True, feedback_fault='bounds')

    def test_redundant_joint_offset_with_arrived_pose_still_releases(self):
        self.check_pipeline(use_waypoints=False, inject_drift=False, release=True, joint_residual='nullspace')

    def test_final_pose_error_prevents_release_and_reports_joint_errors(self):
        self.check_pipeline(use_waypoints=False, inject_drift=False, release=True, joint_residual='wrong_pose')

    def test_two_fk_failures_after_waypoints_recover_before_final_stream(self):
        self.check_pipeline(use_waypoints=True, inject_drift=False, release=True, fk_fault='transient')

    def test_persistent_fk_failure_after_waypoints_stops_without_final_stream_or_release(self):
        self.check_pipeline(use_waypoints=True, inject_drift=False, release=True, fk_fault='persistent')

    def test_recovered_fk_mismatch_still_stops_without_stream_or_release(self):
        self.check_pipeline(use_waypoints=False, inject_drift=False, release=True, fk_fault='mismatch')

    def test_stale_feedback_during_fk_retries_stops_without_stream_or_release(self):
        self.check_pipeline(use_waypoints=False, inject_drift=False, release=True, fk_fault='stale')

    def check_pipeline(self, use_waypoints, inject_drift, release, feedback_fault=None, joint_residual=None,
                       point_cloud=None, fk_fault=None):
        config = copy.deepcopy(TASK)
        # All topic and service names are isolated as well as the ROS domain.
        namespace = "/moveit_pipeline_test"
        config.update(execute_task=True, task_mode="pregrasp", robot_namespace=namespace,
                      moveit_point_cloud_enabled=point_cloud is not None,
                      moveit_point_cloud_topic=namespace+'/points', moveit_point_cloud_timeout_ms=2000,
                      approach_place_release_enabled=release,
                      large_target_topic=namespace+"/large", slots_topic=namespace+"/slots",
                      sequence_topic=namespace+"/sequence", event_service=namespace+"/set_state",
                      grip_settle_ms=20, waypoint_settle_ms=0, pose_arrival_timeout_ms=2000,
                      service_timeout_ms=2000, vision_wait_timeout_ms=5000,
                      approach_reference_z_rpy=[-.365, 1.083344, .122257, -1.185933])
        wp2 = [2.681,3.158,-2.315,-1.074,1.293,-.427,.174]
        # All cases lift to the same measured FK; avoid a fake pose/joint mismatch.
        config['approach_grasp_z_m'] = -.29747956270668224-.12
        config['approach_reference_z_rpy'] = [-.30,1.58515865252804,-.0194000230397028,-2.723737994155044]
        if use_waypoints:
            wp1 = list(wp2); wp1[0] -= .02
            config['approach_place_route'] = {
                'names': ['transfer_1','transfer_2'],
                'transfer_1': wp1, 'transfer_2': wp2}
        else:
            config.pop('approach_place_route', None)
            # Fixture lift ends at known model FK so current joints and pose agree.
            config['approach_grasp_z_m'] = -.29747956270668224-.12
            config['approach_reference_z_rpy'] = [-.30,1.58515865252804,-.0194000230397028,-2.723737994155044]
        config.update(MODEL.parameters(config))
        urdf = ET.fromstring(config["robot_description"])
        by_child = {j.find("child").get("link"): j for j in urdf.findall("joint")}
        chain = []; child = "arm_left_L8_Link"
        while child != "base_link":
            j = by_child[child]; chain.append(j); child = j.find("parent").get("link")
        chain.reverse()

        def fk(q):
            t = np.eye(4); index = 0
            for j in chain:
                origin = j.find("origin"); fixed = np.eye(4)
                if origin is not None:
                    fixed[:3, 3] = [float(v) for v in origin.get("xyz", "0 0 0").split()]
                    fixed[:3, :3] = rotation(*[float(v) for v in origin.get("rpy", "0 0 0").split()])
                t = t @ fixed
                if j.get("type") == "revolute":
                    axis = np.array([float(v) for v in j.find("axis").get("xyz").split()])
                    x, y, z = axis; skew = np.array([[0, -z, y], [z, 0, -x], [-y, x, 0]])
                    turn = np.eye(4); turn[:3, :3] = np.eye(3) + math.sin(q[index])*skew + (1-math.cos(q[index]))*(skew@skew)
                    t = t @ turn; index += 1
            r = t[:3, :3]
            return [*t[:3, 3], math.atan2(r[2,1], r[2,2]), math.atan2(-r[2,0], math.hypot(r[0,0], r[1,0])), math.atan2(r[1,0], r[0,0])]

        rclpy.init()
        node = rclpy.create_node("moveit_pipeline_fake_driver")
        prefix = namespace + "/left_arm/"
        q = [0.]*7; actual = fk(q); events = []; follow_times = []; drift_applied = False
        post_lift_fk_count = 0; fk_fault_finished = False; fk_fault_queries = []
        released_poses = []
        lifted_z = -.29747956270668224
        nut_xy = [.3671314749738187,-.13827291580578394]
        slot_xy = [nut_xy[0]+.01,nut_xy[1]]
        state_pub = node.create_publisher(JointState, prefix+"joint_states", 10)
        pose_pub = node.create_publisher(PoseStamped, prefix+"pose_states", 10)
        nut_pub = node.create_publisher(PointStamped, namespace+"/large", 10)
        slot_pub = node.create_publisher(PoseArray, namespace+"/slots", 10)
        cloud_pub = node.create_publisher(PointCloud2, namespace+'/points', 1)
        cloud_points = [[float(x), float(y), -.7] for x in np.linspace(.15, .75, 31) for y in np.linspace(-.6, .6, 41)]
        obstacle_cloud = create_cloud_xyz32(Header(frame_id='base_link'), cloud_points)

        def tick():
            stamp = node.get_clock().now().to_msg()
            # After enter, the camera is occluded: planning must keep its snapshot.
            if point_cloud == 'valid' and 'MoveJ' not in events:
                obstacle_cloud.header.stamp = stamp; cloud_pub.publish(obstacle_cloud)
            state = JointState(); state.header.stamp = stamp; state.position = q
            fault_active = len(follow_times) >= 10
            if feedback_fault == 'bounds' and fault_active:
                state.position[1] = config['left_joint_max'][1]+.0005
            if (feedback_fault != 'stale' or not fault_active) and not (fk_fault == 'stale' and fk_fault_queries):
                state_pub.publish(state)
            pose = PoseStamped(); pose.header.frame_id = "base_link"; pose.header.stamp = stamp
            pose.pose.position.x, pose.pose.position.y, pose.pose.position.z = actual[:3]
            r, p, y = actual[3:]; cr, sr, cp, sp, cy, sy = math.cos(r/2), math.sin(r/2), math.cos(p/2), math.sin(p/2), math.cos(y/2), math.sin(y/2)
            pose.pose.orientation.x, pose.pose.orientation.y = sr*cp*cy-cr*sp*sy, cr*sp*cy+sr*cp*sy
            pose.pose.orientation.z, pose.pose.orientation.w = cr*cp*sy-sr*sp*cy, cr*cp*cy+sr*sp*sy
            pose_pub.publish(pose)
            nut = PointStamped(); nut.header = pose.header
            nut.point.x, nut.point.y, nut.point.z = *nut_xy, -.60
            nut_pub.publish(nut)
            slots = PoseArray(); slots.header = pose.header
            for x, y in ((.20, -.15), (.28, -.20), slot_xy):
                p = Pose(); p.position.x=x; p.position.y=y; p.position.z=-.498; p.orientation.w=1.; slots.poses.append(p)
            slot_pub.publish(slots)

        def movej(req, resp):
            events.append("MoveJ"); q[:] = req.joints; actual[:] = fk(q); resp.success=True; return resp

        def cartesian(kind):
            def callback(req, resp):
                events.append(kind)
                actual[:] = [req.position.x, req.position.y, req.position.z, req.euler.x, req.euler.y, req.euler.z]
                if kind == 'MoveL' and events.count('MoveL') == 2:
                    q[:] = wp2
                resp.success=True; return resp
            return callback

        def forward(req, resp):
            nonlocal drift_applied, post_lift_fk_count, fk_fault_finished
            mismatched = False
            if events.count('MoveL') == 2:
                post_lift_fk_count += 1
                # First 3 FK checks belong to waypoint 1, next 3 to waypoint 2.
                # Fail the final slot segment's first check, as in the field log.
                trigger = 7 if use_waypoints else 1
                if fk_fault and not fk_fault_finished and post_lift_fk_count >= trigger:
                    fk_fault_queries.append((list(req.joints), len(follow_times), time.monotonic()))
                    failures = 2 if fk_fault == 'transient' else 1
                    if fk_fault in ('persistent', 'stale') or len(fk_fault_queries) <= failures:
                        resp.success = False
                        return resp
                    fk_fault_finished = True
                    mismatched = fk_fault == 'mismatch'
            if inject_drift and events.count('MoveL') == 2 and not drift_applied:
                q[3] += .012
                actual[:] = fk(q)
                drift_applied = True
                events.append('start_drift')
            value = fk(req.joints)
            if mismatched: value[0] += .1
            resp.position.x, resp.position.y, resp.position.z = value[:3]
            resp.euler.x, resp.euler.y, resp.euler.z = value[3:]; resp.success=True; return resp

        def stop(req, resp):
            events.append("stop"); resp.success=True; return resp

        def follow(msg):
            self.assertTrue(msg.follow); self.assertEqual(len(msg.joints), 7)
            if not follow_times:
                self.assertLess(max(abs(a-b) for a,b in zip(q,msg.joints)),.005)
                events.append("joint_follow")
            follow_times.append(time.monotonic()); q[:] = msg.joints
            if joint_residual == 'nullspace':
                # Independently derive a local redundant direction from the
                # fixture FK. Joint error exceeds .005 while XYZ/RPY barely move.
                nominal = np.array(fk(q)); jacobian = np.zeros((6,7))
                for index in range(7):
                    perturbed = list(q); perturbed[index] += 1e-6
                    jacobian[:,index] = (np.array(fk(perturbed))-nominal)/1e-6
                direction = np.linalg.svd(jacobian, full_matrices=True)[2][-1]
                direction *= .008/np.max(np.abs(direction))
                alternatives = [np.array(q)+sign*direction for sign in (1.,-1.)]
                bounded = [value for value in alternatives if all(lo <= a <= hi for a,lo,hi in zip(
                    value,config['left_joint_min'],config['left_joint_max']))]
                self.assertTrue(bounded, 'fixture redundant state outside joint bounds')
                q[:] = bounded[0].tolist()
            elif joint_residual == 'wrong_pose':
                q[6] += .04
            actual[:] = fk(q)

        def hand(msg):
            values = tuple(msg.data)
            if values == (240,30,180,180,180,180) and follow_times:
                released_poses.append(list(actual))
            events.append(values)

        resources = [node.create_service(kind, prefix+name, callback) for kind, name, callback in (
            (MoveJ,"move_joint",movej), (MoveJP,"move_pose",cartesian("MoveJP")),
            (MoveL,"move_linear",cartesian("MoveL")), (ForwardKinematics,"forward_kinematics",forward),
            (SetEmergency,"set_emergency_stop",stop))]
        resources += [node.create_subscription(FollowJoint, prefix+"joint_follow", follow, 10),
                      node.create_subscription(UInt8MultiArray, namespace+"/left_hand/set_l6_joint", hand, 10),
                      node.create_timer(.01, tick)]
        process = None
        try:
            with tempfile.TemporaryDirectory(prefix="lbot_moveit_pipeline_") as directory:
                path = Path(directory)
                (path/"params.yaml").write_text(yaml.safe_dump({"/**":{"ros__parameters":config}}, allow_unicode=True))
                with (path/"output.log").open("w+") as log:
                    process = subprocess.Popen([EXECUTABLE,"--ros-args","--params-file",str(path/"params.yaml")], stdout=log,stderr=subprocess.STDOUT)
                    deadline = time.monotonic()+50
                    while process.poll() is None and time.monotonic()<deadline:
                        rclpy.spin_once(node, timeout_sec=.005)
                    if process.poll() is None: process.kill(); process.wait()
                    log.seek(0); output = log.read()
                # The final hand_open command is unconditional after actual motion;
                # failures publish it before requesting stop. Slot release is separate.
                cleanup_open = tuple(config['hand_open'])
                if 'MoveJ' in events:
                    if process.returncode == 0:
                        self.assertEqual(events[-3:], [cleanup_open] * 3, output)
                        del events[-3:]
                    else:
                        self.assertEqual(events[-4:], [cleanup_open] * 3 + ['stop'], output)
                        self.assertEqual(events.count('stop'), 1, output)
                        del events[-4:-1]
                else:
                    self.assertNotIn(cleanup_open, events)
                if fk_fault:
                    self.assertGreaterEqual(len(fk_fault_queries), 1 if fk_fault == 'stale' else 2, output)
                    for request, count, _ in fk_fault_queries:
                        self.assertEqual(request, fk_fault_queries[0][0], 'FK retries changed requested joints')
                        self.assertEqual(count, fk_fault_queries[0][1], 'trajectory sent while FK verification was pending')
                    for a,b in zip(fk_fault_queries, fk_fault_queries[1:]):
                        self.assertGreaterEqual(b[2]-a[2], .14, output)
                    if use_waypoints:
                        self.assertIn('reached LARGE post-lift transfer waypoint 2/2', output)
                        self.assertGreater(fk_fault_queries[0][1], 0)
                    if fk_fault == 'transient':
                        self.assertEqual(len(fk_fault_queries), 3, output)
                        self.assertIn('pre-transfer FK query recovered', output)
                        self.assertIn('on attempt 3/3', output)
                    else:
                        self.assertNotEqual(process.returncode, 0, output)
                        self.assertIn('stop', events)
                        self.assertEqual(len(follow_times), fk_fault_queries[0][1], output)
                        self.assertFalse(released_poses, events)
                        self.assertNotIn('LARGE release at saved slot', output)
                        self.assertIn('no joint_follow command sent for this segment', output)
                        if fk_fault == 'persistent':
                            self.assertEqual(len(fk_fault_queries), 3, output)
                            self.assertIn('pre-transfer FK verification failed at sample 1/', output)
                            self.assertIn('attempt 3/3', output)
                        elif fk_fault == 'mismatch':
                            self.assertEqual(len(fk_fault_queries), 2, output)
                            self.assertIn('MoveIt / controller FK mismatch at sample 1/', output)
                        else:
                            self.assertLessEqual(len(fk_fault_queries), 2, output)
                            self.assertIn('pre-transfer FK retry stopped: fresh bounded left joint feedback unavailable', output)
                        return
                if feedback_fault:
                    self.assertNotEqual(process.returncode, 0, output)
                    self.assertGreaterEqual(len(follow_times), 10, output)
                    self.assertIn('stop', events)
                    self.assertFalse(released_poses, events)
                    self.assertNotIn('LARGE release at saved slot', output)
                    self.assertIn('age=', output)
                    self.assertIn('last_joints=[', output)
                    if feedback_fault == 'stale':
                        self.assertIn('feedback timeout (limit=200 ms)', output)
                    else:
                        self.assertIn('measured joint limit violation; J2 actual=3.2005', output)
                    return
                if point_cloud == 'missing':
                    self.assertNotEqual(process.returncode, 0, output)
                    self.assertIn('point-cloud snapshot failed before motion', output)
                    self.assertFalse([event for event in events if event != 'stop'], events)
                    self.assertFalse(released_poses)
                    return
                if joint_residual == 'wrong_pose':
                    self.assertNotEqual(process.returncode, 0, output)
                    self.assertIn('stop', events)
                    self.assertFalse(released_poses, events)
                    self.assertNotIn('LARGE release at saved slot', output)
                    self.assertIn('final joints did not settle:', output)
                    self.assertIn('J7 target=', output)
                    self.assertIn('measured FK xyz=', output)
                    return
                self.assertEqual(process.returncode, 0, output)
                if point_cloud == 'valid':
                    self.assertIn('point-cloud snapshot ready', output)
                    self.assertLess(output.index('point-cloud snapshot ready'), output.index('starting taught MoveJ enter route'))
                if joint_residual == 'nullspace':
                    self.assertIn('completed with measured FK pose arrival', output)
                    self.assertIn('waiting for actual pose after slot transfer', output)
                self.assertGreater(len(follow_times), 50, output)
                self.assertEqual(events.count("MoveJP"), 1, events)
                self.assertEqual(events.count("MoveL"), 2, events)
                self.assertEqual(events.count("MoveJ"), 2, events)
                self.assertEqual(output.count('MoveIt transfer height check:'),
                                 4 if inject_drift else (3 if use_waypoints else 1), output)
                if use_waypoints:
                    self.assertIn('reached LARGE post-lift transfer waypoint 1/2 (transfer_1)', output)
                    self.assertIn('reached LARGE post-lift transfer waypoint 2/2 (transfer_2)', output)
                self.assertNotIn("stop", events)
                hands = [event for event in events if isinstance(event, tuple)]
                if release:
                    self.assertEqual(events[-1], (240,30,180,180,180,180), events)
                    self.assertEqual(set(hands[-3:]), {(240,30,180,180,180,180)}, events)
                    self.assertEqual(len(released_poses), 3, events)
                    self.assertIn("finished at saved slot and released", output)
                    for released in released_poses:
                        palm_at_release = np.array(released[:3]) + rotation(*released[3:]) @ np.array([
                            config['tcp_offset_x_m'],config['tcp_offset_y_m'],config['tcp_offset_z_m']])
                        self.assertAlmostEqual(released[2], lifted_z, delta=.001)
                        self.assertAlmostEqual(palm_at_release[0], slot_xy[0], delta=.001)
                        self.assertAlmostEqual(palm_at_release[1], slot_xy[1], delta=.001)
                else:
                    self.assertEqual(events[-1], "joint_follow", events)
                    self.assertEqual(set(hands[-3:]), {(0,30,0,0,0,0)}, events)
                    self.assertFalse(released_poses)
                    self.assertIn("task cleanup will open hand", output)
                if inject_drift:
                    self.assertTrue(drift_applied)
                    self.assertIn('replanning from settled current joints (1/2)', output)
                    self.assertIn('J4 planned=', output)
                    self.assertIn('selected goal XYZ/RPY unchanged', output)
                self.assertAlmostEqual(actual[2], lifted_z, delta=.001)
                palm = np.array(actual[:3]) + rotation(*actual[3:]) @ np.array([
                    config['tcp_offset_x_m'],config['tcp_offset_y_m'],config['tcp_offset_z_m']])
                self.assertAlmostEqual(palm[0], slot_xy[0], delta=.001)
                self.assertAlmostEqual(palm[1], slot_xy[1], delta=.001)
        finally:
            if process is not None and process.poll() is None: process.kill(); process.wait()
            node.destroy_node(); rclpy.shutdown()


if __name__ == "__main__":
    unittest.main()
