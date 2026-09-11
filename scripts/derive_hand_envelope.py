#!/usr/bin/env python3
"""Conservative O6 envelope over the CAD's whole joint range, no ROS commands.

For each collision mesh, enclose it in a local box. Propagate that box through
every joint to Arm_Tip. For a revolute joint, each parent coordinate has the
form a*cos(q)+b*sin(q)+c: evaluate both endpoints AND every stationary angle.
Mimic joints are bounded independently (a conservative superset), including
both their declared range and the range implied by the driving joint.
"""
import argparse
import hashlib
import itertools
import json
import math
from pathlib import Path
import xml.etree.ElementTree as ET
import numpy as np
from scipy.spatial import ConvexHull


def rotation(rpy):
    r, p, y = rpy
    cr, sr, cp, sp, cy, sy = math.cos(r), math.sin(r), math.cos(p), math.sin(p), math.cos(y), math.sin(y)
    return np.array([[cy*cp, cy*sp*sr-sy*cr, cy*sp*cr+sy*sr],
                     [sy*cp, sy*sp*sr+cy*cr, sy*sp*cr-cy*sr], [-sp, cp*sr, cp*cr]])


def corners(bounds):
    return np.array(list(itertools.product(*zip(bounds[0], bounds[1]))))


def origin(element):
    value = element.find('origin'); t = np.zeros(3); r = np.eye(3)
    if value is not None:
        t = np.fromstring(value.get('xyz', '0 0 0'), sep=' ')
        r = rotation(np.fromstring(value.get('rpy', '0 0 0'), sep=' '))
    return r, t


def revolute_bounds(points, axis, limits, r, t):
    axis = np.asarray(axis, dtype=float); axis /= np.linalg.norm(axis)
    x, y, z = axis
    skew = np.array([[0, -z, y], [z, 0, -x], [-y, x, 0]])
    projection = np.outer(axis, axis)
    a = points @ (r@(np.eye(3)-projection)).T
    b = points @ (r@skew).T
    c = points @ (r@projection).T+t
    lo, hi = limits
    candidates = [a*math.cos(lo)+b*math.sin(lo)+c, a*math.cos(hi)+b*math.sin(hi)+c]
    phase = np.arctan2(b, a)
    # All extrema recur every pi, with alternating max/min.
    for k in range(math.floor((lo-math.pi)/math.pi)-1, math.ceil((hi+math.pi)/math.pi)+2):
        angle = phase+k*math.pi
        candidate = a*np.cos(angle)+b*np.sin(angle)+c
        candidates.append(np.where((angle >= lo)&(angle <= hi), candidate, np.nan))
    stacked = np.concatenate(candidates)
    return np.array([np.nanmin(stacked, axis=0), np.nanmax(stacked, axis=0)])


def write_hull(points, hull, path):
    records = np.zeros(len(hull.simplices), dtype=[('normal','<f4',3),('v','<f4',(3,3)),('attr','<u2')])
    for i, ids in enumerate(hull.simplices):
        triangle = points[ids].copy(); normal = hull.equations[i, :3]
        if np.dot(np.cross(triangle[1]-triangle[0], triangle[2]-triangle[0]), normal) < 0:
            triangle[[1,2]] = triangle[[2,1]]
        records['normal'][i] = normal; records['v'][i] = triangle
    content = b'O6 conservative swept envelope'.ljust(80, b'\0')+len(records).to_bytes(4,'little')+records.tobytes()
    Path(path).write_bytes(content)
    return hashlib.sha256(content).hexdigest()


def derive(source, padding=.01, mesh_directory=None, angle_step=.2):
    source = Path(source).resolve(); root = ET.parse(source).getroot()
    joints = {joint.get('name'): joint for joint in root.findall('joint')}
    parents = {joint.find('child').get('link'): joint for joint in joints.values()}
    link_results = []; meshes = {}; joint_ranges = {}; collision_meshes = []; complete_hand = []
    def joint_range(joint):
        name = joint.get('name')
        if name in joint_ranges: return joint_ranges[name]
        limit = joint.find('limit'); low = float(limit.get('lower')); high = float(limit.get('upper'))
        mimic = joint.find('mimic')
        if mimic is not None:
            driver = joint_range(joints[mimic.get('joint')])
            ends = [q*float(mimic.get('multiplier', '1'))+float(mimic.get('offset', '0')) for q in driver]
            low, high = min(low, *ends), max(high, *ends)
        joint_ranges[name] = [low, high]
        return joint_ranges[name]
    for link in root.findall('link'):
        name = link.get('name')
        if not name.startswith('hand_left_'): continue
        for collision in link.findall('collision'):
            mesh = collision.find('geometry/mesh')
            if mesh is None: raise ValueError('Expected mesh collision for '+name)
            path = (source.parent/mesh.get('filename')).resolve(); raw = path.read_bytes()
            count = int.from_bytes(raw[80:84], 'little')
            if len(raw) != 84+50*count: raise ValueError('Expected binary STL: '+str(path))
            vertices = np.frombuffer(raw, offset=84, dtype=[('normal','<f4',3),('v','<f4',(3,3)),('attr','<u2')])['v'].reshape(-1,3).astype(float)
            vertices *= np.fromstring(mesh.get('scale','1 1 1'), sep=' ')
            r, t = origin(collision); vertices = vertices@r.T+t
            bounds = np.array([vertices.min(0), vertices.max(0)])
            child = name; chain = []
            while child != 'arm_left_L8_Link':
                joint = parents[child]; chain.append(joint)
                child = joint.find('parent').get('link')
            moving = [j for j in chain if j.get('type') == 'revolute']
            partitions = []
            for joint in moving:
                low, high = joint_range(joint)
                samples = np.linspace(low, high, max(1, math.ceil((high-low)/angle_step))+1)
                partitions.append(list(zip(samples[:-1], samples[1:])))
            volumes = []
            for intervals in itertools.product(*partitions):
                active = {j.get('name'): interval for j, interval in zip(moving, intervals)}
                current = bounds.copy()
                for joint in chain:
                    r, t = origin(joint); points = corners(current)
                    if joint.get('type') == 'fixed':
                        values = points@r.T+t; current = np.array([values.min(0), values.max(0)])
                    elif joint.get('type') == 'revolute':
                        axis = np.fromstring(joint.find('axis').get('xyz'), sep=' ')
                        current = revolute_bounds(points, axis, active[joint.get('name')], r, t)
                    else: raise ValueError('Unsupported joint '+joint.get('name'))
                volumes.append(corners(current))
            cloud = np.concatenate(volumes); hull = ConvexHull(cloud)
            # Minkowski sum with a padding cube encloses a padding-radius ball.
            padding_corners = np.array(list(itertools.product([-padding, padding], repeat=3)))
            padded = (cloud[hull.vertices, None, :]+padding_corners[None, :, :]).reshape(-1, 3)
            hull = ConvexHull(padded)
            complete_hand.append(padded[hull.vertices])
            padded_bounds = np.array([padded.min(0), padded.max(0)])
            link_results.append({'link': name, 'bounds_arm_tip_m': padded_bounds.tolist(),
                                 'interval_cells': len(volumes), 'hull_vertices': len(hull.vertices)})
            if mesh_directory is not None:
                directory = Path(mesh_directory); directory.mkdir(parents=True, exist_ok=True)
                mesh_file = directory/(name+'.stl')
                collision_meshes.append({'link': name, 'file': directory.name+'/'+mesh_file.name,
                                         'sha256': write_hull(padded, hull, mesh_file)})
            meshes[mesh.get('filename')] = hashlib.sha256(raw).hexdigest()
    if len(link_results) != 12: raise ValueError('Expected 12 O6 left-hand collision meshes')
    low = np.min([v['bounds_arm_tip_m'][0] for v in link_results], axis=0)
    high = np.max([v['bounds_arm_tip_m'][1] for v in link_results], axis=0)
    combined = np.concatenate(complete_hand); hull = ConvexHull(combined)
    collision_hull = None
    if mesh_directory is not None:
        path = Path(mesh_directory)/'hand_swept_envelope.stl'
        collision_hull = {'file': path.parent.name+'/'+path.name, 'sha256': write_hull(combined, hull, path),
                          'vertices': len(hull.vertices)}
    return {'source_urdf': str(source), 'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
            'frame': 'arm_left_L8_Link / Arm_Tip', 'padding_m': padding,
            'method': 'per-link convex hull of analytically enclosed joint subintervals; independent mimic supersets; cube Minkowski padding',
            'angle_step_rad': angle_step, 'collision_meshes': collision_meshes, 'collision_hull': collision_hull,
            'bounds_m': [low.tolist(),high.tolist()], 'envelope': [*((low+high)/2).tolist(), *(high-low).tolist()],
            'joint_ranges_rad': joint_ranges, 'links': link_results, 'mesh_sha256': meshes,
            'limitations': 'CAD mount and joint ranges assumed correct; no finger feedback; cables and grasped-object geometry not reconstructed.'}


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workstation', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--padding', type=float, default=.01)
    args = parser.parse_args()
    if not math.isfinite(args.padding) or args.padding < 0: parser.error('padding must be finite and nonnegative')
    report = derive(args.workstation, args.padding, args.output.parent/'left_hand_collision_meshes')
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2, ensure_ascii=False)+'\n')
    print(json.dumps({key: report[key] for key in ['bounds_m','envelope','padding_m','method']}, indent=2))
