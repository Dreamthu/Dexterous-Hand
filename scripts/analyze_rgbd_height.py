#!/usr/bin/env python3
"""Offline, auditable plane-relative height statistics for a recorded RGB-D scene."""
import argparse
import csv
import json
from pathlib import Path

import cv2
import numpy as np


def summarize(values):
    values = np.asarray(values)
    values = values[np.isfinite(values)]
    if not values.size:
        return {'count': 0}
    return {'count': int(values.size), 'mean': float(np.mean(values)),
            'std': float(np.std(values)),
            **{f'p{q}': float(np.percentile(values, q)) for q in [5, 25, 50, 75, 95]}}


def fit_plane(points, rng):
    if len(points) < 200:
        raise ValueError('Too few table points')
    sample = points[rng.choice(len(points), min(4000, len(points)), replace=False)]
    best = np.zeros(len(sample), dtype=bool)
    for _ in range(200):
        trio = sample[rng.choice(len(sample), 3, replace=False)]
        normal = np.cross(trio[1] - trio[0], trio[2] - trio[0])
        length = np.linalg.norm(normal)
        if length < 1e-9:
            continue
        normal /= length
        inside = np.abs((sample - trio[0]) @ normal) < 5.0
        if inside.sum() > best.sum():
            best = inside
    if best.mean() < .7:
        raise ValueError('No reliable dominant table plane')
    inliers = sample[best]
    for _ in range(3):
        center = inliers.mean(axis=0)
        _, _, vt = np.linalg.svd(inliers - center, full_matrices=False)
        normal = vt[-1]
        offset = -center @ normal
        inliers = points[np.abs(points @ normal + offset) < 5.0]
    if offset < 0:
        normal, offset = -normal, -offset
    return normal, float(offset), float(len(inliers) / len(points))


def analyze(directory, roi_file):
    capture = json.loads((directory / 'capture.json').read_text())
    definition = json.loads(roi_file.read_text())
    frames = capture['frames']
    if len(frames) < 30:
        raise ValueError('At least 30 pairs are required')
    first = cv2.imread(str(directory / frames[0]['color_file']))
    h, w = first.shape[:2]
    yy, xx = np.mgrid[:h, :w]
    masks = {}
    for name, region in definition['regions'].items():
        mask = np.zeros((h, w), np.uint8)
        for polygon in region.get('polygons', []):
            cv2.fillPoly(mask, [np.array(polygon, np.int32)], 255)
        if 'line' in region:
            a, b = region['line']
            cv2.line(mask, tuple(a), tuple(b), 255, region.get('width', 3))
        if 'gray_max' in region:
            mask[cv2.cvtColor(first, cv2.COLOR_BGR2GRAY) > region['gray_max']] = 0
        if region.get('erode', 0):
            k = region['erode'] * 2 + 1
            mask = cv2.erode(mask, np.ones((k, k), np.uint8))
        masks[name] = mask.astype(bool)
    for name, region in definition['regions'].items():
        if 'intersect' in region:
            masks[name] = np.logical_and.reduce([masks[k] for k in region['intersect']])
        for exclusion in region.get('exclude', []):
            masks[name] &= ~cv2.dilate(masks[exclusion].astype(np.uint8),
                                      np.ones((15, 15), np.uint8)).astype(bool)
    if not masks['table_fit'].any():
        raise ValueError('Empty plane fitting ROI')
    # Reserve spatial tiles for evaluating table noise, never fit those pixels.
    holdout_tiles = ((xx // 8 + yy // 8) % 3) == 0
    masks['table_check'] = masks['table_fit'] & holdout_tiles
    masks['table_fit'] &= ~holdout_tiles
    np.savez_compressed(directory / 'region_masks.npz', **masks)
    overlay = first.copy()
    palette = {'table_check': (0, 200, 0), 'large_ring': (0, 0, 255),
               'left_ring': (0, 180, 255), 'right_ring': (255, 0, 255),
               'left_border': (255, 160, 0), 'left_border_contact': (255, 255, 0)}
    for name, color in palette.items():
        if name not in masks:
            continue
        overlay[masks[name]] = np.array(color, np.uint8)
    cv2.imwrite(str(directory / 'regions.png'), overlay)

    pools = {k: [] for k in masks if k != 'table_fit'}
    rows, planes, height_frames, motion = [], [], [], []
    rng = np.random.default_rng(20260911)
    reference = cv2.cvtColor(first, cv2.COLOR_BGR2GRAY)[225:326, 65:180].astype(np.float32)
    cal = capture['camera_info_latest']
    kd = np.array(cal['depth']['k']).reshape(3, 3)
    kc = np.array(cal['color']['k']).reshape(3, 3)
    dc = np.array(cal['color']['d'])
    dd = np.array(cal['depth']['d'])
    if np.any(np.abs(dd) > 1e-12):
        raise ValueError('This analysis expects rectified D2C depth with zero distortion')
    if cal['depth']['header']['frame_id'] != cal['color']['header']['frame_id']:
        raise ValueError('RGB and depth do not share the optical frame')
    pixels = np.dstack((xx, yy)).astype(np.float64).reshape(-1, 1, 2)
    normalized = cv2.undistortPoints(pixels, kc, dc).reshape(h, w, 2)
    u = np.rint(normalized[..., 0] * kd[0, 0] + kd[0, 2]).astype(int)
    v = np.rint(normalized[..., 1] * kd[1, 1] + kd[1, 2]).astype(int)
    valid_map = (u >= 0) & (u < w) & (v >= 0) & (v < h)
    u, v = np.clip(u, 0, w - 1), np.clip(v, 0, h - 1)
    rays = np.dstack(((u - kd[0, 2]) / kd[0, 0], (v - kd[1, 2]) / kd[1, 1], np.ones((h, w))))
    threshold = definition['height_threshold_mm']
    seen_color, seen_depth = set(), set()
    for record in frames:
        if record['color_stamp_ns'] in seen_color or record['depth_stamp_ns'] in seen_depth:
            raise ValueError('Duplicate timestamp in paired dataset')
        seen_color.add(record['color_stamp_ns'])
        seen_depth.add(record['depth_stamp_ns'])
        if record['delta_ms'] > capture['max_delta_ms']:
            raise ValueError('Pair exceeds timestamp tolerance')
        if record['depth_shape'] != [h, w] or record['color_shape'][:2] != [h, w]:
            raise ValueError('Image sizes changed')
        # Camera calibration is allowed to have different header timestamps,
        # but geometric parameters must stay unchanged during this recording.
        for stream, entry in record['camera_info'].items():
            for field in ('k', 'd', 'width', 'height'):
                if entry[field] != cal[stream][field]:
                    raise ValueError('Calibration changed during capture')
        depth = np.load(directory / record['depth_file']).astype(np.float64)
        if record['depth_encoding'] == '32FC1':
            depth *= 1000
        elif record['depth_encoding'] != '16UC1':
            raise ValueError('Unsupported depth encoding')
        depth = depth[v, u]
        valid = valid_map & np.isfinite(depth) & (depth > 150) & (depth < 5000)
        points = rays * depth[..., None]
        normal, offset, inlier_ratio = fit_plane(points[masks['table_fit'] & valid], rng)
        heights = points @ normal + offset
        heights[~valid] = np.nan
        planes.append({'normal': normal.tolist(), 'offset_mm': offset, 'fit_inlier_ratio': inlier_ratio})
        height_frames.append(heights.astype(np.float32))
        image = cv2.imread(str(directory / record['color_file']))
        gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)[225:326, 65:180].astype(np.float32)
        shift, response = cv2.phaseCorrelate(reference, gray)
        motion.append({'shift_px': list(shift), 'response': response})
        for name in pools:
            mask = masks[name]
            values = heights[mask & valid]
            pools[name].append(values)
            stats = summarize(values)
            row = {'frame': record['index'], 'region': name, 'roi_pixels': int(mask.sum()),
                   'valid_ratio': float(values.size / mask.sum()) if mask.sum() else None,
                   'above_threshold_ratio': float(np.mean(values > threshold)) if values.size else None,
                   **stats}
            rows.append(row)
    np.savez_compressed(directory / 'height_maps_mm.npz', heights=np.stack(height_frames))
    regions = {}
    for name, chunks in pools.items():
        selected = [r for r in rows if r['region'] == name]
        combined = np.concatenate(chunks)
        regions[name] = {
            'roi_pixels': int(masks[name].sum()), 'height_mm': summarize(combined),
            'valid_ratio': float(len(combined) / (masks[name].sum() * len(frames))) if masks[name].sum() else None,
            'frame_median_mm': summarize([r['p50'] for r in selected if r['count']]),
            'frame_above_threshold_ratio': summarize([r['above_threshold_ratio'] for r in selected if r['count']]),
            'above_threshold_ratio': float(np.mean(combined > threshold)) if combined.size else None,
            'height_support_frames': sum(
                r['count'] >= 30 and r['valid_ratio'] >= .7 and
                r['p50'] > threshold and r['above_threshold_ratio'] >= .6
                for r in selected),
        }
    result = {'paired_frames': len(frames), 'threshold_mm': threshold,
              'duration_seconds': (frames[-1]['color_stamp_ns'] - frames[0]['color_stamp_ns']) / 1e9,
              'delta_ms': summarize([r['delta_ms'] for r in frames]), 'regions': regions,
              'planes': planes, 'motion': motion,
              'max_source_crop_shift_px': float(max(np.linalg.norm(m['shift_px']) for m in motion)),
              'alignment': 'Same optical frame and K; RGB undistorted to rectified depth grid; nearest-neighbor sampling.',
              'large_border_overlap_observed': definition['large_border_overlap_observed'],
              'height_support_gate': {'min_valid_pixels': 30, 'min_valid_ratio': .7,
                                     'min_median_height_mm': threshold,
                                     'min_above_threshold_ratio': .6,
                                     'meaning': 'Offline height evidence only; not nut identity, footprint or grasp approval'},
              'notes': definition['notes']}
    (directory / 'height_statistics.json').write_text(json.dumps(result, indent=2, ensure_ascii=False))
    fields = sorted({k for row in rows for k in row})
    with (directory / 'height_per_frame.csv').open('w') as stream:
        writer = csv.DictWriter(stream, fieldnames=fields)
        writer.writeheader()
        writer.writerows(rows)
    print(json.dumps({k: v for k, v in result.items() if k not in ('planes', 'motion')}, indent=2, ensure_ascii=False))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('directory', type=Path)
    parser.add_argument('--regions', type=Path, required=True)
    args = parser.parse_args()
    analyze(args.directory, args.regions)
