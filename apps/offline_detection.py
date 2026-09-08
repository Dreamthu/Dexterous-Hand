#!/usr/bin/env python3
"""Build, test and replay the shared detector without ROS or hardware."""
from __future__ import annotations
import argparse
from pathlib import Path
import sys
import signal

ROOT = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(ROOT))

try:
    from linkerbot.offline import build, collect_frames, replay, run_command, settings  # noqa: E402
    from linkerbot.runtime import ConfigurationError  # noqa: E402
except ModuleNotFoundError as error:
    if error.name != "yaml":
        raise
    print("Missing PyYAML for this Python interpreter; install it before running offline tools.", file=sys.stderr)
    raise SystemExit(2) from error


class Terminated(KeyboardInterrupt):
    """Translate SIGTERM into normal child cleanup and a conventional exit code."""


def terminate(signum, frame):
    raise Terminated()


def main() -> int:
    signal.signal(signal.SIGTERM, terminate)
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="action", required=True)
    for action in ("build", "run", "test"):
        subparser = subparsers.add_parser(action)
        subparser.add_argument("--config", default="config/vision/offline.yaml")
        if action == "run":
            subparser.add_argument("inputs", nargs="*")
            subparser.add_argument("--manifest")
            subparser.add_argument("--output", help="New directory below artifacts/; never overwritten")
            subparser.add_argument("--dry-run", action="store_true")
    args = parser.parse_args()
    try:
        config = settings(args.config)
        if args.action == "build":
            build(config)
        elif args.action == "test":
            run_command([sys.executable, "-m", "unittest", "discover", "-s", "tests", "-p", "test_offline*.py", "-v"], cwd=ROOT)
            if not (config["build_directory"] / "CTestTestfile.cmake").is_file():
                raise ConfigurationError("C++ tests not built; run scripts/build_offline.sh (or .ps1) first")
            run_command(["ctest", "--test-dir", str(config["build_directory"]), "-C", "Release", "--output-on-failure", "--no-tests=error"])
        else:
            return replay(config, collect_frames(args.inputs, args.manifest), args.output, args.dry_run)
    except (ConfigurationError, OSError, ValueError) as error:
        parser.error(str(error))
    except Terminated:
        return 143
    except KeyboardInterrupt:
        return 130
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
