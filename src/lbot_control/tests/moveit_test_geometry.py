"""Independent URDF FK for checking exported trajectories in planner tests."""
import math
import itertools
from pathlib import Path
from urllib.parse import unquote, urlparse
import xml.etree.ElementTree as ET
import numpy as np


def rotation(r, p, y):
    cr, sr, cp, sp, cy, sy = math.cos(r), math.sin(r), math.cos(p), math.sin(p), math.cos(y), math.sin(y)
    return np.array([[cy*cp, cy*sp*sr-sy*cr, cy*sp*cr+sy*sr],
                     [sy*cp, sy*sp*sr+cy*cr, sy*sp*cr-cy*sr], [-sp, cp*sr, cp*cr]])


class Geometry:
    def __init__(self, urdf, box):
        root = ET.fromstring(urdf)
        by_child = {j.find('child').get('link'): j for j in root.findall('joint')}
        chain = []; child = 'arm_left_L8_Link'
        while child != 'base_link':
            joint = by_child[child]; chain.append(joint); child = joint.find('parent').get('link')
        self.chain = []
        for joint in reversed(chain):
            origin = joint.find('origin'); fixed = np.eye(4)
            if origin is not None:
                fixed[:3, 3] = list(map(float, origin.get('xyz', '0 0 0').split()))
                fixed[:3, :3] = rotation(*map(float, origin.get('rpy', '0 0 0').split()))
            axis = None
            if joint.get('type') == 'revolute':
                x, y, z = map(float, joint.find('axis').get('xyz').split())
                axis = np.array([[0, -z, y], [z, 0, -x], [-y, x, 0]])
            self.chain.append((fixed, axis))
        collision = root.find("link[@name='left_hand_envelope']/collision")
        mesh = collision.find('geometry/mesh')
        if mesh is not None:
            raw = Path(unquote(urlparse(mesh.get('filename')).path)).read_bytes()
            count = int.from_bytes(raw[80:84], 'little')
            assert len(raw) == 84+50*count
            vertices = np.frombuffer(raw, offset=84, dtype=[('normal','<f4',3),
                ('v','<f4',(3,3)), ('attr','<u2')])['v'].reshape(-1, 3).astype(float)
            vertices *= np.fromstring(mesh.get('scale', '1 1 1'), sep=' ')
            vertices = np.unique(vertices, axis=0)
        else:
            size = np.fromstring(collision.find('geometry/box').get('size'), sep=' ')
            vertices = np.array(list(itertools.product(*zip(-size/2, size/2))))
        origin = collision.find('origin')
        if origin is not None:
            vertices = vertices @ rotation(*map(float, origin.get('rpy', '0 0 0').split())).T
            vertices += np.fromstring(origin.get('xyz', '0 0 0'), sep=' ')
        self.hand_vertices = vertices

    def transform(self, joints):
        t = np.eye(4); index = 0
        for fixed, axis in self.chain:
            t = t @ fixed
            if axis is not None:
                q = joints[index]; turn = np.eye(4)
                turn[:3, :3] = np.eye(3)+math.sin(q)*axis+(1-math.cos(q))*(axis@axis)
                t = t @ turn; index += 1
        return t

    def heights(self, joints):
        t = self.transform(joints); r = t[:3, :3]
        return np.array([t[2, 3], np.min(self.hand_vertices @ r[2, :]+t[2, 3])])

    def pose(self, joints):
        t = self.transform(joints); r = t[:3, :3]
        return list(map(float, [*t[:3, 3], math.atan2(r[2, 1], r[2, 2]),
                math.atan2(-r[2, 0], math.hypot(r[0, 0], r[1, 0])), math.atan2(r[1, 0], r[0, 0])]))
