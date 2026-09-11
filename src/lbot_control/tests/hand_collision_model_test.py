"""Independently check that the installed hull encloses articulated CAD geometry."""
import importlib.util
import itertools
import json
from pathlib import Path
import unittest
import xml.etree.ElementTree as ET
import numpy as np
from scipy.spatial import ConvexHull
import yaml
from moveit_test_geometry import Geometry, rotation

PACKAGE = Path(__file__).resolve().parents[1]
SPEC = importlib.util.spec_from_file_location('moveit_config', PACKAGE/'python/moveit_config.py')
MODEL = importlib.util.module_from_spec(SPEC); SPEC.loader.exec_module(MODEL)
CONFIG = PACKAGE.parents[1]/'config/control'
TASK = yaml.safe_load((CONFIG/'nut_task.yaml').read_text())['/**']['ros__parameters']


def transform(element):
    t = np.eye(4); origin = element.find('origin')
    if origin is not None:
        t[:3, 3] = np.fromstring(origin.get('xyz', '0 0 0'), sep=' ')
        t[:3, :3] = rotation(*map(float, origin.get('rpy', '0 0 0').split()))
    return t


class HandCollisionModelTest(unittest.TestCase):
    def test_hull_encloses_cad_with_padding_across_full_independent_joint_ranges(self):
        model = Geometry(MODEL.parameters(dict(TASK, moveit_hand_collision_model='cad_swept'))[
            'robot_description'], TASK['moveit_hand_envelope'])
        planes = ConvexHull(model.hand_vertices).equations
        report = json.loads((CONFIG/'left_hand_collision_envelope.json').read_text())
        self.assertGreaterEqual(report['padding_m'], .01)
        source = MODEL.model_source(); root = ET.parse(source).getroot()
        joints = {j.get('name'): j for j in root.findall('joint')}
        by_child = {j.find('child').get('link'): j for j in joints.values()}
        def limits(j):
            value = j.find('limit'); low, high = float(value.get('lower')), float(value.get('upper'))
            mimic = j.find('mimic')
            if mimic is not None:
                ends = [v*float(mimic.get('multiplier', '1'))+float(mimic.get('offset', '0'))
                        for v in limits(joints[mimic.get('joint')])]
                low, high = min(low, *ends), max(high, *ends)
            return low, high
        rng = np.random.default_rng(7391); count = 0
        for link in root.findall('link'):
            if not link.get('name').startswith('hand_left_'): continue
            for collision in link.findall('collision'):
                mesh = collision.find('geometry/mesh'); raw = (source.parent/mesh.get('filename')).read_bytes()
                vertices = np.frombuffer(raw, offset=84, dtype=[('n','<f4',3),
                    ('v','<f4',(3,3)), ('a','<u2')])['v'].reshape(-1,3).astype(float)
                vertices *= np.fromstring(mesh.get('scale', '1 1 1'), sep=' ')
                # Every STL vertex lies in this link-local box. Check its corners
                # via ordinary FK, independent of the generator's interval math.
                points = np.array(list(itertools.product(*zip(vertices.min(0), vertices.max(0)))))
                points = np.column_stack([points, np.ones(8)])
                chain = []; child = link.get('name')
                while child != 'arm_left_L8_Link':
                    joint = by_child[child]; chain.insert(0, joint); child = joint.find('parent').get('link')
                moving = [j for j in chain if j.get('type') == 'revolute']
                ranges = [limits(j) for j in moving]
                samples = list(itertools.product(*ranges))
                samples += [tuple(rng.uniform(lo, hi) for lo, hi in ranges) for _ in range(128)]
                for sample in samples:
                    values = dict(zip((j.get('name') for j in moving), sample)); t = np.eye(4)
                    for joint in chain:
                        t = t @ transform(joint)
                        if joint.get('type') == 'revolute':
                            axis = np.fromstring(joint.find('axis').get('xyz'), sep=' ')
                            axis /= np.linalg.norm(axis); x,y,z = axis
                            skew = np.array([[0,-z,y], [z,0,-x], [-y,x,0]])
                            q = values[joint.get('name')]; turn = np.eye(4)
                            turn[:3,:3] = np.eye(3)+np.sin(q)*skew+(1-np.cos(q))*(skew@skew)
                            t = t @ turn
                    actual = (points @ (t @ transform(collision)).T)[:,:3]
                    # Unit-normal plane distance proves a 10 mm ball about each
                    # point fits inside the serialized float32 collision hull.
                    worst = np.max(actual @ planes[:,:3].T+planes[:,3])
                    self.assertLessEqual(worst, -report['padding_m']+2e-7,
                                         (link.get('name'), sample, worst))
                count += 1
        self.assertEqual(count, 12)

    def test_recorded_lift_pose_does_not_use_empty_bounding_box_corners(self):
        q = [.2153044,.1222248,-1.1900511,-1.6939039,1.3453884,-.7631416,-.5857553]
        heights = {}
        for mode in ('box', 'cad_swept'):
            geometry = Geometry(MODEL.parameters(dict(TASK, moveit_hand_collision_model=mode))[
                'robot_description'], TASK['moveit_hand_envelope'])
            heights[mode] = geometry.heights(q)[1]
        self.assertAlmostEqual(heights['cad_swept'], -.446255113, delta=1e-6)
        self.assertGreater(heights['cad_swept']-heights['box'], .025)


if __name__ == '__main__':
    unittest.main()
