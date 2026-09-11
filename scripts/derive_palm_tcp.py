#!/usr/bin/env python3
"""Derive a left palm surface reference from the O6 model; never sends ROS commands.

The model has no authored palm-centre frame. This geometric convention takes
the hand-base mesh's Y/Z bounding-box centre and the outermost +X surface hit.
+X is the palm side: positive MCP rotation about +Y bends +Z fingers toward +X.
This is a model reference, not a measured grasp-centre calibration.
"""

import argparse
import hashlib
import json
from pathlib import Path
import xml.etree.ElementTree as ET

import numpy as np


def rotation(rpy):
    r, p, y = rpy
    cr, cp, cy = np.cos([r, p, y])
    sr, sp, sy = np.sin([r, p, y])
    return np.array([[cy*cp, cy*sp*sr-sy*cr, cy*sp*cr+sy*sr],
                     [sy*cp, sy*sp*sr+cy*cr, sy*sp*cr-cy*sr],
                     [-sp, cp*sr, cp*cr]])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--workstation', type=Path, required=True)
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--plot', type=Path)
    args = parser.parse_args()
    source = args.workstation.resolve()
    root = ET.parse(source).getroot()
    joint = root.find("joint[@name='mount_hand_left_to_arm_left_wrist_mount']")
    assert joint is not None and joint.get('type') == 'fixed'
    assert joint.find('parent').get('link') == 'arm_left_L8_Link'
    hand = joint.find('child').get('link')
    visual = root.find(f"link[@name='{hand}']/visual")
    origin = visual.find('origin')
    # The supplied model's hand mesh is expressed directly in the hand frame.
    assert np.allclose(np.fromstring(origin.get('xyz'), sep=' '), 0)
    assert np.allclose(np.fromstring(origin.get('rpy'), sep=' '), 0)
    mesh_relative = visual.find('geometry/mesh').get('filename')
    mesh = (source.parent / mesh_relative).resolve()
    raw = mesh.read_bytes()
    count = int.from_bytes(raw[80:84], 'little')
    assert len(raw) == 84 + 50 * count, 'Expected binary STL'
    triangles = np.frombuffer(raw, offset=84, dtype=[
        ('normal', '<f4', 3), ('v', '<f4', (3, 3)), ('attr', '<u2')])['v'].astype(float)
    bounds = np.array([triangles.min((0, 1)), triangles.max((0, 1))])
    yz = bounds.mean(0)[1:]
    a = triangles[:, 1, 1:] - triangles[:, 0, 1:]
    b = triangles[:, 2, 1:] - triangles[:, 0, 1:]
    delta = yz - triangles[:, 0, 1:]
    determinant = a[:, 0]*b[:, 1] - a[:, 1]*b[:, 0]
    ids = np.flatnonzero(np.abs(determinant) > 1e-14)
    u = (delta[ids, 0]*b[ids, 1] - delta[ids, 1]*b[ids, 0]) / determinant[ids]
    v = (a[ids, 0]*delta[ids, 1] - a[ids, 1]*delta[ids, 0]) / determinant[ids]
    inside = (u >= -1e-8) & (v >= -1e-8) & (u+v <= 1+1e-8)
    hit = triangles[ids[inside]]
    xs = hit[:, 0, 0] + u[inside]*(hit[:, 1, 0]-hit[:, 0, 0]) + v[inside]*(hit[:, 2, 0]-hit[:, 0, 0])
    assert len(xs), 'No palm surface intersection'
    palm = np.array([xs.max(), *yz])
    mount_xyz = np.fromstring(joint.find('origin').get('xyz'), sep=' ')
    mount_rpy = np.fromstring(joint.find('origin').get('rpy'), sep=' ')
    offset = mount_xyz + rotation(mount_rpy) @ palm
    report = {
        'status': 'model-derived geometric reference; not measured calibration',
        'source_model': 'Dexterous-Hand/开发资源/assets/workstations/lkls73_i1_o6_bimanual/workstation.urdf',
        'source_urdf_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
        'mesh_relative_to_workstation': mesh_relative,
        'mesh_sha256': hashlib.sha256(raw).hexdigest(),
        'method': 'hand-base mesh Y/Z bounding-box centre, outermost +X surface intersection',
        'hand_frame': hand, 'controller_frame_assumption': 'Arm_Tip = arm_left_L8_Link (zero tool offset)',
        'mesh_bounds_m': bounds.tolist(), 'palm_in_hand_frame_m': palm.tolist(),
        'mount_xyz_m': mount_xyz.tolist(), 'mount_rpy_rad': mount_rpy.tolist(),
        'palm_offset_in_arm_tip_m': offset.tolist(),
        'orientation': 'translation-only TCP; keep controller Arm_Tip RPY unchanged',
    }
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_text(json.dumps(report, indent=2, ensure_ascii=False)+'\n')
    print(json.dumps(report, indent=2, ensure_ascii=False))
    if args.plot:
        import matplotlib
        matplotlib.use('Agg')
        import matplotlib.pyplot as plt
        from matplotlib.collections import PolyCollection
        fig, axes = plt.subplots(1, 2, figsize=(10, 6))
        for ax, dims, title in [(axes[0], (1, 2), 'Palm side (+X view)'),
                                (axes[1], (0, 2), 'Side view')]:
            # Draw mesh triangle projections, with the centre reference marked.
            faces = triangles[:, :, list(dims)] * 1000
            ax.add_collection(PolyCollection(faces, facecolors='#bdc9d4', edgecolors='none'))
            ax.scatter(palm[dims[0]]*1000, palm[dims[1]]*1000, c='#d62728', s=65, zorder=3)
            ax.annotate('Palm reference', palm[list(dims)]*1000, xytext=(8, 8), textcoords='offset points')
            ax.autoscale_view()
            ax.set_aspect('equal')
            ax.set(xlabel='XYZ'[dims[0]]+' (mm)', ylabel='Z (mm)', title=title)
        fig.suptitle('O6 left hand-base mesh: model-derived palm reference')
        fig.tight_layout()
        args.plot.parent.mkdir(parents=True, exist_ok=True)
        fig.savefig(args.plot, dpi=150)
        plt.close(fig)


if __name__ == '__main__':
    main()
