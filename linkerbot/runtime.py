"""Hide ROS process details behind stable application-level commands."""

from __future__ import annotations

import os
from pathlib import Path
import sys
from typing import Any, Dict, List, Mapping, Tuple

import yaml


REPOSITORY_ROOT = Path(__file__).resolve().parents[1]


class ConfigurationError(RuntimeError):
    """Raised when repository configuration cannot produce a safe command."""


def repository_path(value: str | Path) -> Path:
    path = Path(value).expanduser()
    return path.resolve() if path.is_absolute() else (REPOSITORY_ROOT / path).resolve()


def load_yaml(path: str | Path) -> Dict[str, Any]:
    resolved = repository_path(path)
    if not resolved.is_file():
        raise ConfigurationError(f"Configuration file does not exist: {resolved}")
    with resolved.open(encoding="utf-8") as stream:
        document = yaml.safe_load(stream)
    if not isinstance(document, dict):
        raise ConfigurationError(f"Configuration root must be a mapping: {resolved}")
    return document


def required_mapping(document: Mapping[str, Any], key: str) -> Mapping[str, Any]:
    value = document.get(key)
    if not isinstance(value, dict):
        raise ConfigurationError(f"'{key}' must be a mapping")
    return value


def ros_value(value: Any) -> str:
    if isinstance(value, bool):
        return "true" if value else "false"
    if isinstance(value, (str, int, float)):
        return str(value)
    raise ConfigurationError(
        f"Unsupported ROS launch value {value!r}; use a string, number, or boolean"
    )


def camera_command(
    config_path: str | Path = "config/camera/gemini2.yaml",
) -> Tuple[List[str], Dict[str, str]]:
    config = load_yaml(config_path)
    driver = required_mapping(config, "driver")
    parameters = required_mapping(config, "parameters")
    package = driver.get("package")
    launch_file = driver.get("launch_file")
    if not isinstance(package, str) or not isinstance(launch_file, str):
        raise ConfigurationError("camera driver package and launch_file must be strings")

    command = ["ros2", "launch", package, launch_file]
    command.extend(f"{name}:={ros_value(value)}" for name, value in parameters.items())

    color_info_file = config.get("color_info_file")
    if color_info_file:
        calibration_path = repository_path(str(color_info_file))
        if not calibration_path.is_file():
            raise ConfigurationError(
                f"Configured color_info_file does not exist: {calibration_path}"
            )
        command.append(f"color_info_url:={calibration_path.as_uri()}")

    environment = os.environ.copy()
    log_directory = repository_path(str(driver.get("log_directory", "artifacts/logs/orbbec")))
    log_directory.mkdir(parents=True, exist_ok=True)
    environment["ROS_LOG_DIR"] = str(log_directory)
    return command, environment


def detector_command(
    config_path: str | Path = "config/vision/nut_detector.yaml",
    *,
    require_built: bool = True,
) -> List[str]:
    executable = REPOSITORY_ROOT / "install/lbot_vision/lib/lbot_vision/nut_detector_node"
    parameters = repository_path(config_path)
    if not parameters.is_file():
        raise ConfigurationError(f"Detector parameter file does not exist: {parameters}")
    if require_built and not executable.is_file():
        raise ConfigurationError("Detector is not built. Run ./scripts/build.sh first.")
    return [str(executable), "--ros-args", "--params-file", str(parameters)]


def viewer_command(
    config_path: str | Path = "config/viewer/image.yaml",
    *,
    topic: str | None = None,
) -> List[str]:
    config = load_yaml(config_path)
    viewer = required_mapping(config, "viewer")
    package = viewer.get("package")
    executable_name = viewer.get("executable")
    configured_topic = viewer.get("topic")
    if not isinstance(package, str) or not isinstance(executable_name, str):
        raise ConfigurationError("viewer package and executable must be strings")
    selected_topic = topic if topic is not None else configured_topic
    if selected_topic is not None and not isinstance(selected_topic, str):
        raise ConfigurationError("viewer topic must be a string or null")

    try:
        from ament_index_python.packages import get_package_prefix

        package_prefix = Path(get_package_prefix(package))
    except (ImportError, LookupError) as error:
        raise ConfigurationError(
            f"ROS package '{package}' is unavailable. Install ros-jazzy-rqt-image-view."
        ) from error

    executable = package_prefix / "lib" / package / executable_name
    if not executable.is_file() or not os.access(executable, os.X_OK):
        raise ConfigurationError(f"Viewer executable does not exist: {executable}")
    command = [str(executable)]
    if selected_topic:
        command.append(selected_topic)
    return command


def extrinsic_calibration_command(arguments: List[str]) -> List[str]:
    script = REPOSITORY_ROOT / "tools/camera_calibration/calibrate_extrinsics.py"
    if not script.is_file():
        raise ConfigurationError(f"Extrinsic calibration tool does not exist: {script}")
    return [sys.executable, str(script), *arguments]
