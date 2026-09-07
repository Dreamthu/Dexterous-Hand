"""Metric ArUco board geometry and an A4 print at a fixed physical scale."""

from __future__ import annotations

import math
from collections.abc import Mapping
from pathlib import Path

import cv2
import numpy as np
import yaml
from PIL import Image, ImageDraw, ImageFont


DEFAULT_BOARD = {
    "dictionary": "DICT_6X6_250",
    "columns": 3,
    "rows": 3,
    "marker_length_m": 0.04,
    "marker_separation_m": 0.01,
    "first_id": 0,
}
PAGE_MM = (210.0, 297.0)
PRINT_DPI = 254
PIXELS_PER_MM = 10
GRID_TOP_MM = 45.0


def validate_board(spec: Mapping) -> dict:
    """Return validated parameters; marker length includes the black border."""
    if not isinstance(spec, Mapping):
        raise ValueError("Board specification must be a mapping")
    unknown = set(spec) - set(DEFAULT_BOARD)
    if unknown:
        raise ValueError(f"Unknown board parameters: {sorted(unknown)}")
    result = {**DEFAULT_BOARD, **spec}
    if not hasattr(cv2, "aruco"):
        raise RuntimeError("OpenCV with the aruco module is required")
    name = result["dictionary"]
    if not isinstance(name, str) or not name.startswith("DICT_"):
        raise ValueError("dictionary must name an OpenCV predefined dictionary")
    dictionary_id = getattr(cv2.aruco, name, None)
    if not isinstance(dictionary_id, int):
        raise ValueError(f"Unknown ArUco dictionary: {name}")
    dictionary = cv2.aruco.getPredefinedDictionary(dictionary_id)
    for key in ("columns", "rows", "first_id"):
        value = result[key]
        if isinstance(value, bool) or not isinstance(value, int):
            raise ValueError(f"{key} must be an integer")
        if value < (0 if key == "first_id" else 1):
            raise ValueError(f"Invalid {key}: {value}")
    for key in ("marker_length_m", "marker_separation_m"):
        value = result[key]
        if isinstance(value, bool) or not isinstance(value, (int, float)):
            raise ValueError(f"{key} must be a positive finite number in meters")
        if not math.isfinite(value) or value <= 0:
            raise ValueError(f"{key} must be a positive finite number in meters")
        result[key] = float(value)
    count = result["columns"] * result["rows"]
    if result["first_id"] + count > len(dictionary.bytesList):
        raise ValueError("Board marker IDs exceed the selected dictionary")
    length = result["marker_length_m"]
    separation = result["marker_separation_m"]
    width = result["columns"] * length + (result["columns"] - 1) * separation
    height = result["rows"] * length + (result["rows"] - 1) * separation
    if width > 0.150 + 1e-12 or height > 0.217 + 1e-12:
        raise ValueError("Board must fit within 150 x 217 mm on the A4 page")
    return result


def board_points(spec: Mapping) -> dict[int, np.ndarray]:
    """Return corners TL, TR, BR, BL in meters, with origin at grid bottom-left.

    Viewed from the printed front, +X is right, +Y is up, and +Z is out of
    the paper. IDs increase left to right and then top to bottom.
    """
    board = validate_board(spec)
    length = board["marker_length_m"]
    step = length + board["marker_separation_m"]
    height = board["rows"] * step - board["marker_separation_m"]
    points = {}
    for row in range(board["rows"]):
        for column in range(board["columns"]):
            x = column * step
            y = height - row * step
            marker_id = board["first_id"] + row * board["columns"] + column
            points[marker_id] = np.array(
                [[x, y, 0], [x + length, y, 0],
                 [x + length, y - length, 0], [x, y - length, 0]],
                dtype=np.float64,
            )
    return points


def _font(size: int):
    try:
        return ImageFont.truetype("DejaVuSans.ttf", size)
    except OSError:
        return ImageFont.load_default()


def _fixture_geometry(fixture: Mapping, grid_origin_page_mm: np.ndarray,
                      grid_rectangle_mm: np.ndarray) -> dict:
    if not isinstance(fixture, Mapping):
        raise ValueError("Fixture specification must be a mapping")
    try:
        origin = np.asarray(fixture["flange_origin_in_board_m"], dtype=float)
        holes = np.asarray(fixture["holes_in_flange_m"], dtype=float)
        outline = np.asarray(fixture["outline_in_flange_m"], dtype=float)
        diameter = float(fixture["hole_diameter_m"])
        yaw = float(fixture.get("flange_yaw_in_board_deg", 0.0))
        contact_z = float(fixture.get("contact_z_in_flange_m", 0.0))
    except (KeyError, ValueError, TypeError) as error:
        raise ValueError(f"Invalid fixture geometry: {error}") from error
    if (origin.shape != (3,) or holes.shape != (4, 3) or outline.ndim != 2
            or outline.shape[1] not in (2, 3) or len(outline) < 3):
        raise ValueError("Fixture requires a 3D origin, four 3D hole centers, and a polygon outline")
    if outline.shape[1] == 2:
        outline = np.column_stack((outline, np.zeros(len(outline))))
    if (not np.isfinite(origin).all() or not np.isfinite(holes).all()
            or not np.isfinite(outline).all() or not math.isfinite(diameter) or diameter <= 0):
        raise ValueError("Fixture geometry must be finite and hole diameter positive")
    if (not np.isclose(origin[2], 0, atol=1e-9) or not np.allclose(holes[:, 2], 0, atol=1e-9)
            or not np.allclose(outline[:, 2], 0, atol=1e-9) or not np.isclose(contact_z, 0, atol=1e-9)):
        raise ValueError("Fixture contact face and print geometry must lie in z8=0 at board z=0")
    if not math.isfinite(yaw) or not math.isclose(yaw % 360, 0, abs_tol=1e-9):
        raise ValueError("Printed R8 axis labels require flange_yaw_in_board_deg=0")
    source_hash = fixture.get("source_mesh_sha256")
    if (not isinstance(source_hash, str) or len(source_hash) != 64
            or any(character not in "0123456789abcdefABCDEF" for character in source_hash)):
        raise ValueError("Fixture source_mesh_sha256 must record the source mesh SHA-256")
    for index, hole in enumerate(holes):
        if any(np.linalg.norm(hole - other) < diameter for other in holes[index + 1:]):
            raise ValueError("Fixture hole circles overlap")

    def page_points(points):
        return grid_origin_page_mm + (points[:, :2] + origin[:2]) * [1000, -1000]

    page_outline, page_holes = page_points(outline), page_points(holes)
    radius_mm = diameter * 500
    lower = np.minimum(page_outline.min(axis=0), page_holes.min(axis=0) - radius_mm)
    upper = np.maximum(page_outline.max(axis=0), page_holes.max(axis=0) + radius_mm)
    if np.any(lower < [20, 205]) or np.any(upper > [195, 280]):
        raise ValueError("Fixture must fit in the reserved A4 docking zone: x=20..195, y=205..280 mm")
    grid_low, grid_high = grid_rectangle_mm[:2] - 5, grid_rectangle_mm[2:] + 5
    if (lower[0] < grid_high[0] and upper[0] > grid_low[0]
            and lower[1] < grid_high[1] and upper[1] > grid_low[1]):
        raise ValueError("Fixture overlaps the ArUco grid or its 5 mm quiet margin")
    transform = np.eye(4)
    transform[:3, 3] = origin
    return {
        **dict(fixture),
        "flange_origin_in_board_m": origin.tolist(),
        "holes_in_flange_m": holes.tolist(),
        "outline_in_flange_m": outline.tolist(),
        "hole_diameter_m": diameter,
        "board_from_flange": transform.tolist(),
        "flange_origin_page_mm": (grid_origin_page_mm + origin[:2] * [1000, -1000]).tolist(),
        "holes_in_board_m": (holes + origin).tolist(),
        "holes_page_mm": page_holes.tolist(),
        "outline_page_mm": page_outline.tolist(),
        "hole_ids": [1, 2, 3, 4],
        "contact_plane": "z8=0; +X8 right, +Y8 up, +Z8 out of printed front",
    }


def _draw_fixture(draw: ImageDraw.ImageDraw, fixture: Mapping, label_font, small_font) -> None:
    outline = [tuple(np.rint(np.asarray(point) * PIXELS_PER_MM).astype(int))
               for point in fixture["outline_page_mm"]]
    draw.line([*outline, outline[0]], fill="black", width=3)
    label_x = max(point[0] for point in outline) + 45
    radius = fixture["hole_diameter_m"] * 1000 * PIXELS_PER_MM / 2
    for hole_id, point in zip(fixture["hole_ids"], fixture["holes_page_mm"]):
        x, y = np.asarray(point) * PIXELS_PER_MM
        draw.ellipse((x - radius, y - radius, x + radius, y + radius), outline="black", width=2)
        draw.line([(x - 26, y), (x + 26, y)], fill="black", width=1)
        draw.line([(x, y - 26), (x, y + 26)], fill="black", width=1)
        draw.text((max(x + 60, label_x), y - 17), str(hole_id), fill="black", font=label_font)
    origin_x, origin_y = np.asarray(fixture["flange_origin_page_mm"]) * PIXELS_PER_MM
    draw.line([(origin_x - 22, origin_y), (origin_x + 22, origin_y)], fill="black", width=2)
    draw.line([(origin_x, origin_y - 22), (origin_x, origin_y + 22)], fill="black", width=2)
    draw.text((origin_x + 30, origin_y - 10), "O8", fill="black", font=label_font)
    draw.text((260, 2020), "R8 CIRCULAR DISC SIDE", fill="black", font=small_font)
    draw.text((1520, 2080), "SIDE TAB", fill="black", font=small_font)
    draw.text((1570, 2115), "4 HOLES", fill="black", font=small_font)
    draw.text((1600, 2560), "CONTACT", fill="black", font=small_font)
    draw.text((1600, 2590), "FACE z8=0", fill="black", font=small_font)
    draw.line([(1740, 2460), (1960, 2460)], fill="black", width=3)
    draw.line([(1960, 2460), (1935, 2448)], fill="black", width=3)
    draw.line([(1960, 2460), (1935, 2472)], fill="black", width=3)
    draw.line([(1740, 2460), (1740, 2260)], fill="black", width=3)
    draw.line([(1740, 2260), (1728, 2285)], fill="black", width=3)
    draw.line([(1740, 2260), (1752, 2285)], fill="black", width=3)
    draw.text((1780, 2480), "+X8", fill="black", font=label_font)
    draw.text((1770, 2250), "+Y8", fill="black", font=label_font)


def generate_board(spec: Mapping, output_directory: str | Path,
                   fixture: Mapping | None = None) -> dict[str, Path]:
    """Write a metric PNG, an exact A4 PDF, and matching corner geometry YAML."""
    board = validate_board(spec)
    length_px_float = board["marker_length_m"] * 1000 * PIXELS_PER_MM
    separation_px_float = board["marker_separation_m"] * 1000 * PIXELS_PER_MM
    for value in (length_px_float, separation_px_float):
        if value < 1 or not math.isclose(value, round(value), abs_tol=1e-7):
            raise ValueError("Printable lengths must be positive multiples of 0.1 mm")
    length_px = round(length_px_float)
    separation_px = round(separation_px_float)
    dictionary = cv2.aruco.getPredefinedDictionary(getattr(cv2.aruco, board["dictionary"]))
    if length_px < dictionary.markerSize + 2:
        raise ValueError("Marker is too small to print its code and black border")
    grid_width = board["columns"] * length_px + (board["columns"] - 1) * separation_px
    grid_height = board["rows"] * length_px + (board["rows"] - 1) * separation_px
    page_size = tuple(round(mm * PIXELS_PER_MM) for mm in PAGE_MM)
    page = Image.new("RGB", page_size, "white")
    grid_x = (page_size[0] - grid_width) // 2
    grid_y = round(GRID_TOP_MM * PIXELS_PER_MM)
    bottom_y = grid_y + grid_height
    grid_origin_page_mm = np.array([grid_x, bottom_y]) / PIXELS_PER_MM
    fixture_geometry = None
    if fixture is not None:
        fixture_geometry = _fixture_geometry(
            fixture, grid_origin_page_mm,
            np.array([grid_x, grid_y, grid_x + grid_width, bottom_y]) / PIXELS_PER_MM,
        )
    create_marker = getattr(cv2.aruco, "generateImageMarker", None)
    if create_marker is None:
        create_marker = cv2.aruco.drawMarker
    for row in range(board["rows"]):
        for column in range(board["columns"]):
            marker_id = board["first_id"] + row * board["columns"] + column
            marker = create_marker(dictionary, marker_id, length_px, borderBits=1)
            page.paste(Image.fromarray(marker), (
                grid_x + column * (length_px + separation_px),
                grid_y + row * (length_px + separation_px),
            ))

    draw = ImageDraw.Draw(page)
    heading_font, label_font, small_font = _font(38), _font(26), _font(22)
    draw.text((160, 95), f"A4 calibration board / {board['dictionary']}", fill="black", font=heading_font)
    draw.text((160, 160), "Print: A4 / Actual size / 100% / No fit to page", fill="black", font=label_font)
    length_mm = board["marker_length_m"] * 1000
    separation_mm = board["marker_separation_m"] * 1000
    draw.text((160, 210), f"Outer black square: {length_mm:g} mm; gap: {separation_mm:g} mm", fill="black", font=label_font)
    draw.text((160, 260), "Measure BOTH 100 mm scales after printing. Mount flat on a rigid board.", fill="black", font=small_font)

    # Keep annotations outside the quiet zone of every machine-readable marker.
    draw.line([(grid_x, bottom_y + 50), (grid_x, bottom_y + 100)], fill="black", width=2)
    draw.text((grid_x + 12, bottom_y + 55), "O: grid outer bottom-left (0, 0, 0)", fill="black", font=small_font)

    ruler_y, ruler_x = 2870, 150
    draw.line([(550, ruler_y), (1550, ruler_y)], fill="black", width=2)
    for x in (550, 1550):
        draw.line([(x, ruler_y - 15), (x, ruler_y + 15)], fill="black", width=2)
    draw.text((900, ruler_y - 45), "100 mm", fill="black", font=label_font)
    draw.line([(ruler_x, 650), (ruler_x, 1650)], fill="black", width=2)
    for y in (650, 1650):
        draw.line([(ruler_x - 15, y), (ruler_x + 15, y)], fill="black", width=2)
    vertical_label = Image.new("RGB", (220, 45), "white")
    ImageDraw.Draw(vertical_label).text((0, 0), "100 mm", fill="black", font=label_font)
    page.paste(vertical_label.rotate(90, expand=True), (80, 1040))
    if fixture_geometry is not None:
        _draw_fixture(draw, fixture_geometry, label_font, small_font)
    draw.text((260, 315), "Board: +X right, +Y up, +Z out. IDs increase left-to-right, then down.", fill="black", font=small_font)

    output_directory = Path(output_directory)
    output_directory.mkdir(parents=True, exist_ok=True)
    paths = {"pdf": output_directory / "board_a4.pdf",
             "png": output_directory / "board_a4.png",
             "geometry": output_directory / "board_geometry.yaml"}
    page.save(paths["png"], dpi=(PRINT_DPI, PRINT_DPI))
    page.save(paths["pdf"], "PDF", resolution=PRINT_DPI)
    geometry = {
        "board": board,
        "page_mm": list(PAGE_MM),
        "print_dpi": PRINT_DPI,
        "grid_dimensions_m": [grid_width / 10000, grid_height / 10000],
        "grid_top_left_page_mm": [grid_x / PIXELS_PER_MM, grid_y / PIXELS_PER_MM],
        "grid_origin_page_mm": grid_origin_page_mm.tolist(),
        "page_coordinates": "origin at A4 top-left; x right and y down",
        "origin": "outer bottom-left of complete marker grid, viewed from printed front",
        "axes": {"x": "right", "y": "up", "z": "out of printed front"},
        "corner_order": ["top_left", "top_right", "bottom_right", "bottom_left"],
        "points_m": {marker_id: points.tolist() for marker_id, points in board_points(board).items()},
    }
    if fixture_geometry is not None:
        geometry["fixture"] = fixture_geometry
    paths["geometry"].write_text(yaml.safe_dump(geometry, sort_keys=False), encoding="utf-8")
    return paths
