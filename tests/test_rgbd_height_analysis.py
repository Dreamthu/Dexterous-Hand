"""Numerical checks for the diagnostic's plane units, sign and outlier handling."""
import importlib.util
from pathlib import Path
import unittest

import numpy as np

path = Path(__file__).resolve().parents[1] / 'scripts' / 'analyze_rgbd_height.py'
spec = importlib.util.spec_from_file_location('height_diagnostic', path)
module = importlib.util.module_from_spec(spec)
spec.loader.exec_module(module)


class PlaneAnalysisTest(unittest.TestCase):
    def test_tilted_table_and_thirty_mm_object_with_outliers(self):
        rng = np.random.default_rng(13)
        x = rng.uniform(-300, 300, 3000)
        y = rng.uniform(-100, 150, 3000)
        z = 850 - .3 * y + .08 * x
        points = np.column_stack([x, y, z])
        # Known normal points toward camera. Object heights are perpendicular
        # to this tilted plane, not differences of camera Z coordinates.
        truth = np.array([.08, -.3, -1.])
        truth /= np.linalg.norm(truth)
        clean = points.copy()
        points += rng.normal(0, .5, (len(points), 1)) * truth
        points[:400] += 45 * truth
        normal, offset, inlier_ratio = module.fit_plane(points, rng)
        self.assertGreater(normal @ truth, .9999)
        self.assertGreater(inlier_ratio, .8)
        self.assertLess(np.abs(clean @ normal + offset).max(), .15)
        top = clean + 30 * truth
        self.assertLess(np.abs(top @ normal + offset - 30).max(), .15)

    def test_insufficient_plane_support_is_rejected(self):
        with self.assertRaises(ValueError):
            module.fit_plane(np.zeros((20, 3)), np.random.default_rng(0))

    def test_no_depth_evidence_is_not_zero_height(self):
        self.assertEqual(module.summarize([float('nan')]), {'count': 0})


if __name__ == '__main__':
    unittest.main()
