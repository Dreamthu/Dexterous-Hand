#!/usr/bin/env python3
"""Tune the blue basket HSV mask from a ROS 2 image topic or a still image."""

from __future__ import annotations

import argparse
from datetime import datetime
from pathlib import Path
import sys
import time

import cv2
import numpy as np
import yaml

ROOT = Path(__file__).resolve().parents[1]
WINDOW = "Blue basket HSV"
FIELDS = {
    "H min": ("blue_h_min", 90, 180),
    "H max": ("blue_h_max", 140, 180),
    "S min": ("blue_s_min", 70, 255),
    "S max": ("blue_s_max", 255, 255),
    "V min": ("blue_v_min", 35, 255),
    "V max": ("blue_v_max", 255, 255),
}


def read_parameters(path: Path) -> dict:
    data = yaml.safe_load(path.read_text(encoding="utf-8"))
    parameters = data["nut_detector_node"]["ros__parameters"]
    values = {}
    for field, default, maximum in FIELDS.values():
        value = parameters.get(field, default)
        if type(value) is not int or not 0 <= value <= maximum:
            raise ValueError(f"Invalid {field}: expected an integer in 0..{maximum}")
        values[field] = value
    for channel in "hsv":
        if values[f"blue_{channel}_min"] > values[f"blue_{channel}_max"]:
            raise ValueError(f"blue_{channel}_min must be <= blue_{channel}_max")
    return values


def masks(frame: np.ndarray, values: dict) -> tuple:
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    raw = cv2.inRange(
        hsv,
        (values["blue_h_min"], values["blue_s_min"], values["blue_v_min"]),
        (values["blue_h_max"], values["blue_s_max"], values["blue_v_max"]),
    )
    closed = cv2.morphologyEx(
        raw, cv2.MORPH_CLOSE, cv2.getStructuringElement(cv2.MORPH_RECT, (7, 7))
    )
    return hsv, raw, closed


def save_parameters(directory: Path, values: dict) -> Path:
    directory.mkdir(parents=True, exist_ok=True)
    destination = directory / f"blue_hsv_{datetime.now():%Y%m%d_%H%M%S_%f}.yaml"
    document = {"nut_detector_node": {"ros__parameters": values}}
    with destination.open("x", encoding="utf-8") as stream:
        yaml.safe_dump(document, stream, sort_keys=False)
    print(f"Saved {destination}\nCopy these six values into config/vision/nut_detector.yaml:", flush=True)
    print(yaml.safe_dump(values, sort_keys=False), flush=True)
    return destination


class RosImageSource:
    def __init__(self, topic: str):
        try:
            import rclpy
            from cv_bridge import CvBridge
            from rclpy.qos import qos_profile_sensor_data
            from sensor_msgs.msg import Image
        except ImportError as error:
            raise RuntimeError(
                "ROS image input needs rclpy/cv_bridge. Source your ROS setup.bash first, "
                "or use --image PATH."
            ) from error
        self.rclpy = rclpy
        self.frame = None
        self.received_at = None
        self.bridge = CvBridge()
        rclpy.init(args=[])
        self.node = rclpy.create_node("blue_hsv_tuner")
        self.subscription = self.node.create_subscription(
            Image, topic, self.on_image, qos_profile_sensor_data
        )

    def on_image(self, message):
        try:
            self.frame = self.bridge.imgmsg_to_cv2(message, desired_encoding="bgr8")
            self.received_at = time.monotonic()
        except Exception as error:
            self.node.get_logger().warning(f"Cannot decode image: {error}")

    def poll(self):
        if not self.rclpy.ok():
            raise KeyboardInterrupt
        self.rclpy.spin_once(self.node, timeout_sec=0.001)
        return self.frame

    def close(self):
        self.node.destroy_node()
        if self.rclpy.ok():
            self.rclpy.shutdown()


def run(args, initial: dict):
    frame = None
    source = None
    if args.image:
        frame = cv2.imread(str(args.image))
        if frame is None:
            raise ValueError(f"Cannot read image: {args.image}")
    else:
        source = RosImageSource(args.topic)

    try:
        cv2.namedWindow(WINDOW, cv2.WINDOW_AUTOSIZE)
        for label, (field, _, maximum) in FIELDS.items():
            cv2.createTrackbar(label, WINDOW, initial[field], maximum, lambda _: None)
        shown_hsv = None
        tile_size = (0, 0)
        sampled = "Click original image to read HSV"

        def on_mouse(event, x, y, flags, userdata):
            nonlocal sampled
            width, height = tile_size
            if event == cv2.EVENT_LBUTTONDOWN and shown_hsv is not None and 0 <= x < width and 0 <= y < height:
                column = min(shown_hsv.shape[1] - 1, int(x * shown_hsv.shape[1] / width))
                row = min(shown_hsv.shape[0] - 1, int(y * shown_hsv.shape[0] / height))
                h, s, v = map(int, shown_hsv[row, column])
                sampled = f"Pixel ({column}, {row}): H={h} S={s} V={v}"
                print(sampled, flush=True)

        cv2.setMouseCallback(WINDOW, on_mouse)
        paused = False
        print("Space: freeze/resume | S: save YAML | R: reset | Q/Esc: quit", flush=True)
        print("All six HSV limits match the detector parameters. Preview is HSV only.", flush=True)
        if source:
            print(f"Waiting for {args.topic} (start the camera separately)...", flush=True)
        while True:
            if source:
                latest = source.poll()
                if not paused and latest is not None:
                    frame = latest
            values = {
                field: cv2.getTrackbarPos(label, WINDOW)
                for label, (field, _, _) in FIELDS.items()
            }
            for channel in "hsv":
                lower, upper = f"blue_{channel}_min", f"blue_{channel}_max"
                if values[lower] > values[upper]:
                    values[upper] = values[lower]
                    cv2.setTrackbarPos(f"{channel.upper()} max", WINDOW, values[upper])
            if frame is None:
                preview = np.zeros((180, 800, 3), dtype=np.uint8)
                cv2.putText(preview, "Waiting for " + args.topic, (12, 70),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.6, (255, 255, 255), 1)
            else:
                shown_hsv, raw, closed = masks(frame, values)
                scale = min(1.0, args.width / frame.shape[1], args.height / frame.shape[0])
                tile_size = (max(1, round(frame.shape[1] * scale)), max(1, round(frame.shape[0] * scale)))
                tiles = []
                for picture, label in (
                    (frame, "Original"),
                    (cv2.cvtColor(raw, cv2.COLOR_GRAY2BGR), "Raw HSV mask"),
                    (cv2.cvtColor(closed, cv2.COLOR_GRAY2BGR), "Mask after 7x7 close"),
                    (cv2.bitwise_and(frame, frame, mask=closed), "Extracted region (HSV only)"),
                ):
                    tile = cv2.resize(picture, tile_size, interpolation=cv2.INTER_NEAREST)
                    cv2.rectangle(tile, (0, 0), (tile_size[0], 27), (0, 0, 0), -1)
                    cv2.putText(tile, label, (8, 19), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (255, 255, 255), 1)
                    tiles.append(tile)
                preview = np.vstack((np.hstack(tiles[:2]), np.hstack(tiles[2:])))
                footer = np.zeros((65, preview.shape[1], 3), dtype=np.uint8)
                state = "FROZEN" if paused else "LIVE" if source else "IMAGE"
                if source and not paused and time.monotonic() - source.received_at > 2:
                    state = "STALE: no recent camera image"
                cv2.putText(footer, state + " | " + sampled, (8, 22),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.45, (0, 255, 255), 1)
                cv2.putText(footer, "Space: freeze | S: save | R: reset | Q/Esc: quit", (8, 48),
                            cv2.FONT_HERSHEY_SIMPLEX, 0.45, (255, 255, 255), 1)
                preview = np.vstack((preview, footer))
            cv2.imshow(WINDOW, preview)
            key = cv2.waitKey(30) & 0xFF
            if key in (27, ord("q"), ord("Q")) or cv2.getWindowProperty(WINDOW, cv2.WND_PROP_VISIBLE) < 1:
                break
            if key == ord(" ") and frame is not None:
                paused = not paused
            elif key in (ord("r"), ord("R")):
                for label, (field, _, _) in FIELDS.items():
                    cv2.setTrackbarPos(label, WINDOW, initial[field])
            elif key in (ord("s"), ord("S")):
                try:
                    save_parameters(args.output_dir, values)
                except OSError as error:
                    print(f"Cannot save parameters: {error}", file=sys.stderr)
    finally:
        if source:
            source.close()
        cv2.destroyAllWindows()


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    inputs = parser.add_mutually_exclusive_group()
    inputs.add_argument("--image", type=Path, help="Still image; does not require ROS")
    inputs.add_argument("--topic", default="/camera/color/image_raw", help="ROS 2 color Image topic (default: %(default)s)")
    parser.add_argument("--config", type=Path, default=ROOT / "config/vision/nut_detector.yaml")
    parser.add_argument("--output-dir", type=Path, default=ROOT / "artifacts/hsv_tuning")
    parser.add_argument("--width", type=int, default=640, help="Maximum preview tile width")
    parser.add_argument("--height", type=int, default=360, help="Maximum preview tile height")
    args = parser.parse_args()
    if args.width < 320 or args.height < 180:
        parser.error("Preview tile size must be at least 320x180")
    try:
        run(args, read_parameters(args.config))
    except KeyboardInterrupt:
        return 130
    except (OSError, ValueError, KeyError, TypeError, RuntimeError, yaml.YAMLError, cv2.error) as error:
        parser.exit(2, f"Error: {error}\n")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
