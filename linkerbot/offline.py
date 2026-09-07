"""ROS-free build/replay adapter; recognition stays in the shared C++ library."""
from __future__ import annotations

from datetime import datetime, timezone
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import signal
import subprocess
import uuid

from linkerbot.runtime import ConfigurationError, REPOSITORY_ROOT, load_yaml, repository_path

IMAGE_SUFFIXES = {".png", ".jpg", ".jpeg", ".bmp", ".tif", ".tiff"}


def write_json(path: Path, data: object) -> None:
    path.write_text(json.dumps(data, ensure_ascii=False, indent=2, allow_nan=False) + "\n", encoding="utf-8")


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def inside(path: Path, directory: Path, label: str) -> Path:
    if path == directory or directory not in path.parents:
        raise ConfigurationError(f"{label} must be a child of {directory}: {path}")
    return path


def settings(path: str | Path) -> dict:
    config = load_yaml(path)
    expected = {"detector_config", "build_directory", "output_directory", "build_jobs"}
    if set(config) != expected:
        raise ConfigurationError(f"Offline config must contain exactly {sorted(expected)}: {path}")
    for key in expected - {"build_jobs"}:
        if not isinstance(config[key], str) or not config[key].strip():
            raise ConfigurationError(f"{key} must be a nonempty path")
        config[key] = repository_path(config[key])
    jobs = config["build_jobs"]
    if type(jobs) is not int or not 1 <= jobs <= 8:
        raise ConfigurationError("build_jobs must be an integer between 1 and 8")
    inside(config["build_directory"], (REPOSITORY_ROOT / "build").resolve(), "build_directory")
    inside(config["output_directory"], (REPOSITORY_ROOT / "artifacts").resolve(), "output_directory")
    return config


def detector_parameters(path: Path) -> dict:
    """Check transport types; C++ DetectorConfig::validate owns numerical constraints."""
    document = load_yaml(path)
    try:
        params = document["nut_detector_node"]["ros__parameters"]
    except (KeyError, TypeError) as error:
        raise ConfigurationError(f"Missing nut_detector_node.ros__parameters: {path}") from error
    if not isinstance(params, dict):
        raise ConfigurationError(f"ros__parameters must be a mapping: {path}")
    schema = (REPOSITORY_ROOT / "src/lbot_vision/include/lbot_vision/detector_fields.inc").read_text(encoding="utf-8")
    fields = re.findall(r"LBOT_DETECTOR_FIELD\((int|double|std::string), (\w+)\)", schema)
    if not fields:
        raise ConfigurationError("Empty detector field schema")
    effective = {}
    for kind, name in fields:
        value = params.get(name)
        if kind == "std::string":
            valid = isinstance(value, str)
        elif kind == "int":
            valid = type(value) is int and -(2**31) <= value < 2**31
        else:
            valid = type(value) in (int, float) and math.isfinite(value)
        if not valid:
            raise ConfigurationError(f"{path}: {name} must be {kind} (finite, not null/bool)")
        effective[name] = value
    return effective


def executable(config: dict) -> Path:
    suffix = ".exe" if os.name == "nt" else ""
    return config["build_directory"] / "bin" / ("nut_detector_offline" + suffix)


def run_process(command: list[str], *, check: bool = False, **kwargs) -> subprocess.CompletedProcess:
    """Reap the child (and build descendants) on interruption, including SIGTERM."""
    options = {"start_new_session": True} if os.name != "nt" else {
        "creationflags": subprocess.CREATE_NEW_PROCESS_GROUP}
    with subprocess.Popen(command, **options, **kwargs) as process:
        try:
            stdout, stderr = process.communicate()
        except BaseException:
            if process.poll() is None:
                if os.name == "nt":
                    # Only this child tree, never arbitrary user processes.
                    subprocess.run(["taskkill", "/PID", str(process.pid), "/T", "/F"],
                                   stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL, check=False)
                else:
                    try:
                        os.killpg(process.pid, signal.SIGTERM)
                    except ProcessLookupError:
                        pass
                try:
                    process.wait(timeout=3)
                except subprocess.TimeoutExpired:
                    if os.name != "nt":
                        try:
                            os.killpg(process.pid, signal.SIGKILL)
                        except ProcessLookupError:
                            pass
                    else:
                        process.kill()
                    process.wait()
            raise
        result = subprocess.CompletedProcess(command, process.returncode, stdout, stderr)
        if check:
            result.check_returncode()
        return result


def run_command(command: list[str], **kwargs) -> None:
    try:
        run_process(command, check=True, **kwargs)
    except FileNotFoundError as error:
        raise ConfigurationError(f"Required executable not found: {command[0]}") from error
    except subprocess.CalledProcessError as error:
        raise ConfigurationError(f"Command failed ({error.returncode}): {command}") from error


def build(config: dict) -> None:
    directory = config["build_directory"]
    parameters = detector_parameters(config["detector_config"])
    directory.mkdir(parents=True, exist_ok=True)
    effective = directory / "test_detector_config.json"
    write_json(effective, parameters)
    run_command(["cmake", "-S", str(REPOSITORY_ROOT / "tools/offline_detection"),
                 "-B", str(directory), "-DCMAKE_BUILD_TYPE=Release", "-DBUILD_TESTING=ON",
                 f"-DDETECTOR_TEST_CONFIG={effective.as_posix()}"])
    run_command(["cmake", "--build", str(directory), "--config", "Release", "--parallel", str(config["build_jobs"])])
    run_command([str(executable(config)), "--check-config", effective.name], cwd=directory)


def collect_frames(inputs: list[str], manifest: str | None) -> list[dict]:
    frames = []
    if manifest:
        if inputs:
            raise ConfigurationError("Use either image/directory inputs or --manifest, not both")
        path = repository_path(manifest)
        try:
            document = json.loads(path.read_text(encoding="utf-8"))
        except (OSError, ValueError) as error:
            raise ConfigurationError(f"Cannot read sequence manifest {path}: {error}") from error
        entries = document.get("frames") if isinstance(document, dict) else None
        if not isinstance(entries, list) or not entries:
            raise ConfigurationError("Sequence manifest requires a nonempty frames array")
        for entry in entries:
            if not isinstance(entry, dict) or not isinstance(entry.get("image"), str):
                raise ConfigurationError("Each manifest frame requires an image path")
            timestamp = entry.get("capture_timestamp_ns")
            if timestamp is not None and (type(timestamp) is not int or timestamp < 0):
                raise ConfigurationError("capture_timestamp_ns must be a nonnegative integer or null")
            frames.append({"source": (path.parent / entry["image"]).resolve(),
                           "capture_timestamp_ns": timestamp,
                           "metadata": {key: value for key, value in entry.items() if key != "image"}})
    else:
        for value in inputs:
            path = repository_path(value)
            if path.is_dir():
                paths = sorted((item for item in path.iterdir() if item.is_file() and item.suffix.lower() in IMAGE_SUFFIXES),
                               key=lambda item: item.name)
            else:
                paths = [path]
            frames.extend({"source": item, "capture_timestamp_ns": None, "metadata": {}} for item in paths)
    if not frames:
        raise ConfigurationError("No images supplied/found; use files, a directory, or --manifest")
    for frame in frames:
        if not frame["source"].is_file():
            raise ConfigurationError(f"Input image does not exist: {frame['source']}")
    return frames


def source_version() -> dict:
    def git(*args: str) -> str:
        # Command-local trust for this already-selected repository, never global config.
        result = subprocess.run(["git", "-c", f"safe.directory={REPOSITORY_ROOT.as_posix()}",
                                 "-C", str(REPOSITORY_ROOT), *args], check=True, capture_output=True)
        return result.stdout.decode("utf-8", errors="replace").strip()
    try:
        revision = {"commit": git("rev-parse", "HEAD"), "status": git("status", "--porcelain"),
                    "tracked_diff": git("diff", "HEAD", "--", ".")}
    except (OSError, subprocess.CalledProcessError):
        revision = {"commit": None, "status": "git_unavailable", "tracked_diff": ""}
    hashes = {}
    for directory in ("src/lbot_vision", "tools/offline_detection", "linkerbot", "apps", "scripts"):
        for path in sorted((REPOSITORY_ROOT / directory).rglob("*")):
            if path.is_file() and path.suffix in {".cpp", ".hpp", ".inc", ".cmake", ".txt", ".py", ".sh", ".ps1"}:
                hashes[path.relative_to(REPOSITORY_ROOT).as_posix()] = sha256(path)
    revision["source_sha256"] = hashes
    return revision


def replay(config: dict, frames: list[dict], output: str | None, dry_run: bool = False) -> int:
    parameters = detector_parameters(config["detector_config"])
    binary = executable(config)
    run_dir = repository_path(output) if output else config["output_directory"] / (
        datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ") + "_" + uuid.uuid4().hex[:8])
    inside(run_dir, (REPOSITORY_ROOT / "artifacts").resolve(), "output")
    if run_dir.exists():
        raise ConfigurationError(f"Output already exists; refusing to overwrite: {run_dir}")
    if dry_run:
        print(json.dumps({"executable": str(binary), "built": binary.is_file(), "frame_count": len(frames),
                          "output": str(run_dir), "note": "Transport checked; numerical validation occurs in C++ at build/run."},
                         ensure_ascii=False, indent=2))
        return 0
    if not binary.is_file():
        raise ConfigurationError(f"Offline detector not built: {binary}. Run scripts/build_offline.sh (or .ps1).")
    # Run validation before creating a replay artifact directory.
    validation_path = config["build_directory"] / ("validate_" + uuid.uuid4().hex + ".json")
    try:
        write_json(validation_path, parameters)
        run_command([str(binary), "--check-config", validation_path.name], cwd=validation_path.parent)
    finally:
        validation_path.unlink(missing_ok=True)
    run_dir.mkdir(parents=True, exist_ok=False)
    write_json(run_dir / "effective_detector.json", parameters)
    shutil.copyfile(config["detector_config"], run_dir / "detector_config.yaml")
    version = source_version()
    (run_dir / "tracked_changes.patch").write_text(version.pop("tracked_diff"), encoding="utf-8")
    record = {"schema_version": 1, "started_at_utc": datetime.now(timezone.utc).isoformat(),
              "mode": "independent_frames_2d_no_tracking", "version": version,
              "binary_sha256": sha256(binary), "detector_config_sha256": sha256(run_dir / "detector_config.yaml"),
              "effective_detector_sha256": sha256(run_dir / "effective_detector.json"),
              "frame_order": "manifest order or input order; directories lexicographic by filename",
              "state": "running", "frames": []}
    write_json(run_dir / "run.json", record)
    failures = 0
    try:
        for index, frame in enumerate(frames):
            directory = run_dir / f"{index:06d}"
            directory.mkdir()
            # Stage exact encoded bytes under ASCII names. No EXIF rotation, resizing or recompression.
            suffix = frame["source"].suffix.lower()
            input_name = "input" + (suffix if suffix in IMAGE_SUFFIXES else ".bin")
            shutil.copyfile(frame["source"], directory / input_name)
            entry = {"index": index, "source": str(frame["source"]),
                     "input_sha256": sha256(directory / input_name), "saved_input": f"{index:06d}/{input_name}",
                     "capture_timestamp_ns": frame["capture_timestamp_ns"], "metadata": frame["metadata"],
                     "result": f"{index:06d}/result.json"}
            # Relative ASCII argv also supports non-ASCII workspace/input paths on Windows.
            with (directory / "detector.log").open("w", encoding="utf-8") as log:
                process = run_process([str(binary), "../effective_detector.json", input_name],
                                         cwd=directory, stdout=log, stderr=subprocess.STDOUT)
            entry["returncode"] = process.returncode
            if process.returncode != 0:
                failures += 1
                write_json(directory / "result.json", {"status": "processing_error", "localization_valid": False,
                           "position": None, "error_log": "detector.log"})
            else:
                result_path = directory / "result.json"
                result = json.loads(result_path.read_text(encoding="utf-8"))
                for key in ("frame_found", "basket_found", "localization_valid"):
                    result[key] = bool(result[key])
                result["position"] = None
                result["capture_timestamp_ns"] = frame["capture_timestamp_ns"]
                for candidate in result["candidates"]:
                    candidate["accepted"] = bool(candidate["accepted"])
                write_json(result_path, result)
            record["frames"].append(entry)
            write_json(run_dir / "run.json", record)
        record["state"] = "completed_with_errors" if failures else "completed"
    except BaseException:
        record["state"] = "interrupted_or_failed"
        raise
    finally:
        record["finished_at_utc"] = datetime.now(timezone.utc).isoformat()
        write_json(run_dir / "run.json", record)
    print(f"Replay output: {run_dir}; frames={len(frames)}, errors={failures}")
    return 1 if failures else 0
