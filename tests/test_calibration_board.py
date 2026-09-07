from __future__ import annotations

import re
from copy import deepcopy
import tempfile
import unittest

import cv2
import numpy as np
import yaml
from PIL import Image

from linkerbot.calibration_board import DEFAULT_BOARD, board_points, generate_board, validate_board


FIXTURE = {
    "flange_origin_in_board_m": [.020, -.060, 0],
    "holes_in_flange_m": [[.079970359, -.022605121, 0], [.079986787, -.010105133, 0],
                          [.080013073, .009894850, 0], [.080029502, .022394839, 0]],
    "hole_diameter_m": .0035,
    # The fixture tests use a simple representative outline; production uses CAD.
    "outline_in_flange_m": [[-.030, -.030, 0], [.030, -.030, 0], [.030, -.025, 0],
                            [.087, -.025, 0], [.087, .025, 0], [.030, .025, 0],
                            [.030, .030, 0], [-.030, .030, 0]],
    "source_mesh_sha256": "a" * 64,
}


class CalibrationBoardTests(unittest.TestCase):
    def test_metric_corner_order_and_origin(self):
        points = board_points(DEFAULT_BOARD)
        self.assertEqual(list(points), list(range(9)))
        np.testing.assert_allclose(points[0], [[0, .14, 0], [.04, .14, 0],
                                             [.04, .10, 0], [0, .10, 0]])
        np.testing.assert_allclose(points[8], [[.10, .04, 0], [.14, .04, 0],
                                              [.14, 0, 0], [.10, 0, 0]], atol=1e-15)
        self.assertEqual(points[0].dtype, np.float64)
        self.assertLess(np.cross(points[0][1] - points[0][0],
                                 points[0][2] - points[0][1])[2], 0)

    def test_rejects_invalid_dimensions_and_dictionary_ids(self):
        for changes in ({"columns": True}, {"rows": 0}, {"rows": 4.5},
                        {"first_id": -1}, {"first_id": 245},
                        {"marker_length_m": float("nan")}, {"marker_separation_m": 0},
                        {"dictionary": "MISSING"}, {"columns": 10},
                        {"marker_length_m": "0.04"}, {"columns_typo": 3}):
            with self.subTest(changes=changes), self.assertRaises(ValueError):
                validate_board({**DEFAULT_BOARD, **changes})

    def test_generated_page_is_a4_and_detects_matching_ids_and_geometry(self):
        with tempfile.TemporaryDirectory() as directory:
            paths = generate_board(DEFAULT_BOARD, directory, FIXTURE)
            with Image.open(paths["png"]) as page:
                self.assertEqual(page.size, (2100, 2970))
                self.assertAlmostEqual(page.info["dpi"][0], 254, places=2)
            image = cv2.imread(str(paths["png"]), cv2.IMREAD_GRAYSCALE)
            dictionary = cv2.aruco.getPredefinedDictionary(cv2.aruco.DICT_6X6_250)
            if hasattr(cv2.aruco, "ArucoDetector"):
                corners, ids, _ = cv2.aruco.ArucoDetector(dictionary).detectMarkers(image)
            else:
                corners, ids, _ = cv2.aruco.detectMarkers(image, dictionary)
            self.assertEqual(set(ids.flatten()), set(range(9)))
            points = board_points(DEFAULT_BOARD)
            for marker_corners, marker_id in zip(corners, ids.flatten()):
                # Grid starts at page (35, 45) mm; its bottom is y=185 mm.
                projected = np.column_stack((350 + points[marker_id][:, 0] * 10000,
                                             1850 - points[marker_id][:, 1] * 10000))
                np.testing.assert_allclose(marker_corners[0], projected, atol=1.1)
            media_box = re.search(rb"/MediaBox\s*\[([^]]+)\]", paths["pdf"].read_bytes())
            self.assertIsNotNone(media_box)
            bounds = [float(value) for value in media_box.group(1).split()]
            np.testing.assert_allclose(bounds, [0, 0, 210 / 25.4 * 72, 297 / 25.4 * 72], atol=1e-6)
            geometry = yaml.safe_load(paths["geometry"].read_text())
            np.testing.assert_allclose(geometry["points_m"][0], points[0])
            self.assertEqual(geometry["board"], DEFAULT_BOARD)
            np.testing.assert_allclose(geometry["grid_top_left_page_mm"], [35, 45])
            np.testing.assert_allclose(geometry["grid_origin_page_mm"], [35, 185])
            fixture = geometry["fixture"]
            np.testing.assert_allclose(fixture["flange_origin_page_mm"], [55, 245])
            np.testing.assert_allclose(fixture["holes_page_mm"][0], [134.970359, 267.605121])
            np.testing.assert_allclose(fixture["holes_page_mm"][3], [135.029502, 222.605161])
            np.testing.assert_allclose(np.asarray(fixture["board_from_flange"])[:3, 3], [.020, -.060, 0])
            self.assertEqual(fixture["source_mesh_sha256"], FIXTURE["source_mesh_sha256"])
            for hole_page in fixture["holes_page_mm"]:
                x, y = np.rint(np.asarray(hole_page) * 10).astype(int)
                self.assertEqual(image[y, x], 0)
            # Docking geometry cannot obscure tag corners or their quiet zones.
            self.assertGreater(np.min(np.asarray(fixture["outline_page_mm"])[:, 1]), 190)

    def test_fixture_rejects_marker_overlap_bad_plane_and_page_overflow(self):
        variants = []
        for origin in ([.020, .010, 0], [-.080, -.060, 0], [.020, -.120, 0], [.020, -.060, .004]):
            fixture = deepcopy(FIXTURE)
            fixture["flange_origin_in_board_m"] = origin
            variants.append(fixture)
        fixture = deepcopy(FIXTURE)
        fixture["holes_in_flange_m"][0][2] = .010
        variants.append(fixture)
        fixture = deepcopy(FIXTURE)
        fixture["holes_in_flange_m"][0] = fixture["holes_in_flange_m"][1]
        variants.append(fixture)
        fixture = deepcopy(FIXTURE)
        del fixture["source_mesh_sha256"]
        variants.append(fixture)
        for fixture in variants:
            with self.subTest(fixture=fixture), tempfile.TemporaryDirectory() as directory:
                with self.assertRaises(ValueError):
                    generate_board(DEFAULT_BOARD, directory, fixture)

    def test_printing_without_fixture_still_emits_metric_board(self):
        with tempfile.TemporaryDirectory() as directory:
            paths = generate_board(DEFAULT_BOARD, directory)
            geometry = yaml.safe_load(paths["geometry"].read_text())
            self.assertNotIn("fixture", geometry)
            np.testing.assert_allclose(geometry["grid_dimensions_m"], [.140, .140])

    def test_rejects_silent_print_scale_rounding(self):
        with tempfile.TemporaryDirectory() as directory, self.assertRaises(ValueError):
            generate_board({**DEFAULT_BOARD, "marker_length_m": .04001}, directory)


if __name__ == "__main__":
    unittest.main()
