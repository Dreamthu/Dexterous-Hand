"""Load one URDF chain and evaluate measured joints using Orocos KDL."""

from __future__ import annotations

import hashlib
import math
from pathlib import Path
import xml.etree.ElementTree as ET

import numpy as np

from linkerbot.runtime import ConfigurationError


def _vector(element: ET.Element | None, attribute: str, default: str) -> list[float]:
    text = element.get(attribute, default) if element is not None else default
    try:
        value = [float(v) for v in text.split()]
    except ValueError as error:
        raise ConfigurationError(f"Invalid URDF {attribute}: {text}") from error
    if len(value) != 3 or not all(math.isfinite(v) for v in value):
        raise ConfigurationError(f"Invalid URDF {attribute}: {text}")
    return value


class RobotChain:
    def __init__(self, urdf_path: Path, base: str, tip: str):
        try:
            import PyKDL as kdl
        except ImportError as error:
            raise ConfigurationError("Install python3-pykdl and run with the system Python") from error
        self.kdl = kdl
        raw = urdf_path.read_bytes()
        self.urdf_sha256 = hashlib.sha256(raw).hexdigest()
        root = ET.fromstring(raw)
        links = {link.attrib["name"] for link in root.findall("link")}
        if base not in links or tip not in links or base == tip:
            raise ConfigurationError("Requested base/tip frames do not define a URDF chain")
        by_child = {}
        for joint in root.findall("joint"):
            child = joint.find("child").attrib["link"]
            if child in by_child:
                raise ConfigurationError(f"Multiple URDF joints parent link {child}")
            by_child[child] = joint
        joints, seen = [], set()
        current = tip
        while current != base:
            if current in seen or current not in by_child:
                raise ConfigurationError(f"No acyclic chain from {base} to {tip}")
            seen.add(current)
            joint = by_child[current]
            joints.append(joint)
            current = joint.find("parent").attrib["link"]
        self.chain = kdl.Chain()
        self.names, self.limits = [], {}
        for joint in reversed(joints):
            name, kind = joint.attrib["name"], joint.attrib["type"]
            if joint.find("mimic") is not None:
                raise ConfigurationError(f"Mimic joint not supported in calibration chain: {name}")
            origin = joint.find("origin")
            xyz = kdl.Vector(*_vector(origin, "xyz", "0 0 0"))
            frame = kdl.Frame(kdl.Rotation.RPY(*_vector(origin, "rpy", "0 0 0")), xyz)
            if kind == "fixed":
                kdl_joint = kdl.Joint(name, kdl.Joint.Fixed)
            elif kind in ("revolute", "continuous", "prismatic"):
                axis = np.asarray(_vector(joint.find("axis"), "xyz", "1 0 0"))
                if np.linalg.norm(axis) < 1e-12:
                    raise ConfigurationError(f"Zero joint axis: {name}")
                # KDL's axis is in the parent frame; URDF's is in the joint frame.
                parent_axis = frame.M * kdl.Vector(*(axis / np.linalg.norm(axis)))
                joint_type = kdl.Joint.TransAxis if kind == "prismatic" else kdl.Joint.RotAxis
                kdl_joint = kdl.Joint(name, xyz, parent_axis, joint_type)
                self.names.append(name)
                if kind != "continuous":
                    limit = joint.find("limit")
                    if limit is None:
                        raise ConfigurationError(f"Missing joint limits: {name}")
                    lower, upper = float(limit.attrib["lower"]), float(limit.attrib["upper"])
                    if not math.isfinite(lower) or not math.isfinite(upper) or lower > upper:
                        raise ConfigurationError(f"Invalid joint limits: {name}")
                    self.limits[name] = lower, upper
            else:
                raise ConfigurationError(f"Unsupported joint {name}: {kind}")
            self.chain.addSegment(kdl.Segment(joint.find("child").attrib["link"], kdl_joint, frame))
        self.solver = kdl.ChainFkSolverPos_recursive(self.chain)

    def forward(self, document: dict) -> np.ndarray:
        positions = document.get("joint_positions_rad")
        if not isinstance(positions, dict):
            raise ConfigurationError("Joint snapshot requires joint_positions_rad mapping")
        if set(positions) != set(self.names):
            raise ConfigurationError(f"Joint snapshot must contain exactly these URDF names: {self.names}")
        array = self.kdl.JntArray(len(self.names))
        for i, name in enumerate(self.names):
            value = positions[name]
            if isinstance(value, bool) or not isinstance(value, (int, float)) or not math.isfinite(value):
                raise ConfigurationError(f"Enter the actual measured angle in radians for {name}")
            if name in self.limits and not self.limits[name][0] <= value <= self.limits[name][1]:
                raise ConfigurationError(f"{name}={value} lies outside URDF limits; check units/zero/sign")
            array[i] = value
        frame = self.kdl.Frame()
        if self.solver.JntToCart(array, frame) < 0:
            raise ConfigurationError("KDL forward kinematics failed")
        result = np.eye(4)
        for row in range(3):
            result[row, 3] = frame.p[row]
            for column in range(3):
                result[row, column] = frame.M[row, column]
        return result
