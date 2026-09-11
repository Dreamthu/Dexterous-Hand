"""Build a controller-convention left-arm model from the supplied CAD assets.

No right-arm state is assumed. Collision checking covers the torso, left arm
and a configurable rigid hand envelope; the table/basket/right arm are not in
this model. Add measured world obstacles in moveit_obstacle_boxes.
"""
from copy import deepcopy
from pathlib import Path
import math
import hashlib
import json
import xml.etree.ElementTree as ET


def model_source(explicit=""):
    if explicit:
        result = Path(explicit).expanduser().resolve()
        if not result.is_file():
            raise RuntimeError(f"MoveIt CAD model does not exist: {result}")
        return result
    relative = Path("开发资源/assets/workstations/lkls73_i1_o6_bimanual/workstation.urdf")
    starts = [Path(__file__).resolve(), Path.cwd()]
    candidates = [start / relative for start in starts]
    candidates.extend(parent / relative for start in starts for parent in start.parents)
    for candidate in candidates:
        if candidate.is_file():
            return candidate.resolve()
    raise RuntimeError("Cannot find the packaged left-arm CAD model; set moveit_model_source launch argument")


def parameters(task, source=""):
    path = model_source(source)
    original = ET.parse(path).getroot()
    links = {link.get("name"): link for link in original.findall("link")}
    by_child = {j.find("child").get("link"): j for j in original.findall("joint")}
    chain = []
    child = "arm_left_L8_Link"
    while child != "base_base_link":
        joint = deepcopy(by_child[child])
        chain.append(joint)
        child = joint.find("parent").get("link")
    chain.reverse()
    names = ["base_base_link"] + [j.find("child").get("link") for j in chain]
    robot = ET.Element("robot", name="lbot_left_controller")
    for name in names:
        link = deepcopy(links[name])
        if name == "base_base_link":
            link.set("name", "base_link")
            for inertia in link.findall("inertial"):
                link.remove(inertia)  # Fixed root; this model is for kinematics only.
        for mesh in link.findall(".//mesh"):
            mesh_path = (path.parent / mesh.get("filename")).resolve()
            if not mesh_path.is_file():
                raise RuntimeError(f"Missing collision mesh: {mesh_path}")
            mesh.set("filename", mesh_path.as_uri())
        robot.append(link)
    lows = task.get("left_joint_min", [-2.91, -.07, -2.72, -2.05, -2.69, -1.59, -1.59])
    highs = task.get("left_joint_max", [2.91, 3.20, 2.69, 2.05, 2.69, 1.59, 1.59])
    margin = task.get("joint_limit_margin_rad", 0.0)
    speed = task.get("moveit_joint_velocity_rad_s", 0.12)
    accel = task.get("moveit_joint_acceleration_rad_s2", 0.12)
    if len(lows) != 7 or len(highs) != 7 or not all(math.isfinite(v) for v in [*lows, *highs, margin, speed, accel]) or margin < 0 or not 0 < speed <= 1 or not 0 < accel <= 1:
        raise RuntimeError("Invalid MoveIt joint bounds / speed / acceleration")
    limits = {}
    for joint in chain:
        if joint.find("parent").get("link") == "base_base_link":
            joint.find("parent").set("link", "base_link")
        if joint.get("type") == "revolute":
            index = int(joint.get("name").split("_L")[1].split("_")[0]) - 1
            # CAD q = -controller q for ALL seven joints, verified against SDK FK.
            axis = joint.find("axis")
            axis.set("xyz", " ".join(str(-float(v)) for v in axis.get("xyz").split()))
            lo, hi = lows[index] + margin, highs[index] - margin
            if lo >= hi:
                raise RuntimeError("Joint margin consumes joint range")
            joint.find("limit").set("lower", str(lo))
            joint.find("limit").set("upper", str(hi))
            joint.find("limit").set("velocity", str(speed))
            limits[joint.get("name")] = {
                "has_velocity_limits": True, "max_velocity": float(speed),
                "has_acceleration_limits": True, "max_acceleration": float(accel)}
        robot.append(joint)
    box = task.get("moveit_hand_envelope", [0., 0., -.105, .22, .22, .23])
    if len(box) != 6 or not all(math.isfinite(v) for v in box) or any(v <= 0 for v in box[3:]):
        raise RuntimeError("moveit_hand_envelope must be [x,y,z,size_x,size_y,size_z]")
    hand = ET.SubElement(robot, "link", name="left_hand_envelope")
    collision = ET.SubElement(hand, "collision")
    hand_model = task.get("moveit_hand_collision_model", "box")
    if hand_model == "cad_swept":
        candidates = [Path(__file__).resolve().parent.parent / "config/left_hand_collision_envelope.json",
                      Path(__file__).resolve().parents[3] / "config/control/left_hand_collision_envelope.json"]
        manifest = next((candidate for candidate in candidates if candidate.is_file()), None)
        if manifest is None:
            raise RuntimeError("Missing O6 swept collision model; rebuild lbot_control")
        report = json.loads(manifest.read_text())
        if report["source_sha256"] != hashlib.sha256(path.read_bytes()).hexdigest():
            raise RuntimeError("O6 collision envelope is stale for this CAD; regenerate with scripts/derive_hand_envelope.py")
        for relative, expected in report['mesh_sha256'].items():
            if hashlib.sha256((path.parent/relative).read_bytes()).hexdigest() != expected:
                raise RuntimeError("O6 source mesh changed; regenerate the collision envelope")
        mesh = (manifest.parent/report["collision_hull"]["file"]).resolve()
        if hashlib.sha256(mesh.read_bytes()).hexdigest() != report["collision_hull"]["sha256"]:
            raise RuntimeError("O6 collision hull checksum mismatch")
        ET.SubElement(collision, "origin", xyz="0 0 0", rpy="0 0 0")
        ET.SubElement(ET.SubElement(collision, "geometry"), "mesh", filename=mesh.as_uri())
    elif hand_model == "box":
        ET.SubElement(collision, "origin", xyz=" ".join(map(str, box[:3])), rpy="0 0 0")
        ET.SubElement(ET.SubElement(collision, "geometry"), "box", size=" ".join(map(str, box[3:])))
    else:
        raise RuntimeError("moveit_hand_collision_model must be cad_swept or box")
    joint = ET.SubElement(robot, "joint", name="left_hand_envelope_mount", type="fixed")
    ET.SubElement(joint, "parent", link="arm_left_L8_Link")
    ET.SubElement(joint, "child", link="left_hand_envelope")
    semantic = ET.Element("robot", name="lbot_left_controller")
    ET.SubElement(ET.SubElement(semantic, "group", name="left_arm"), "chain",
                  base_link="base_link", tip_link="arm_left_L8_Link")
    physical = ["base_link"] + [f"arm_left_L{i}_Link" for i in range(1, 9)]
    pairs = list(zip(physical, physical[1:])) + [("arm_left_L8_Link", "left_hand_envelope"), ("arm_left_L7_Link", "left_hand_envelope")]
    for a, b in pairs:
        ET.SubElement(semantic, "disable_collisions", link1=a, link2=b, reason="Adjacent")
    return {
        "robot_description": ET.tostring(robot, encoding="unicode"),
        "robot_description_semantic": ET.tostring(semantic, encoding="unicode"),
        "robot_description_kinematics": {"left_arm": {
            "kinematics_solver": "kdl_kinematics_plugin/KDLKinematicsPlugin",
            "kinematics_solver_search_resolution": 0.05,
            "kinematics_solver_timeout": 0.08}},
        "robot_description_planning": {"joint_limits": limits},
        "ompl": {"planning_plugins": ["ompl_interface/OMPLPlanner"],
                 "planner_configs": {"RRTConnect": {"type": "geometric::RRTConnect", "range": 0.1}},
                 "left_arm": {"planner_configs": ["RRTConnect"], "longest_valid_segment_fraction": 0.001,
                              # Multiple hand-corner constraints are checked by
                              # joint-space sampling, not a single-tip IK sampler.
                              "enforce_joint_model_state_space": True}},
    }
