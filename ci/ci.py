from ci.build import build, clean
from ci.artifacts import stage_artifacts
from ci.archive import archive

from pathlib import Path
import argparse
import logging
import sys
import subprocess
import shutil

# Logger
logger = logging.getLogger("ci")
logger.setLevel(logging.DEBUG)

# Argument parser
parser = argparse.ArgumentParser(description="CI/CD Script")
parser.add_argument(
    "-d", "--debug",
    dest="debug",
    action="store_true",
    help="Enable debug build"
)
parser.add_argument(
    "-c", "--clean",
    dest="clean",
    action="store_true",
    help="Clean builds"
)
parser.add_argument(
    "-a", "--archive",
    dest="archive",
    action="store_true",
    help="Archive the projekt"
)

parser.add_argument(
    "--no-build",
    dest="build",
    action="store_false",
    help="Stop before building the project"
)
parser.add_argument(
    "--no-test",
    dest="test",
    action="store_false",
    help="Stop before testing the project"
)
parser.add_argument(
    "--no-log",
    dest="log",
    action="store_false",
    help="Stops logging while building"
)

parser.add_argument(
    "-A", "--archive-name",
    dest="archive_name",
    metavar="NAME",
    type=str,
    help="Name of the archive to create"
)

def main(args) -> bool:
    if (args.clean):
        clean(debug=args.debug)
        shutil.rmtree("dist", ignore_errors=True)
        shutil.rmtree("archives", ignore_errors=True)

    if (not args.build):
        logger.debug("Stopping before building")
        return False

    result: bool = build(debug=args.debug)
    if (not result):
        logger.error("Building failed")
        return False

    result = stage_artifacts(debug=args.debug)
    if (not result):
        logger.error("Staging artifacts failed")
        return False

    if (not args.test):
        logger.debug("Stopping before testing")
        return True

    logger.info("Starting tests")
    ret = subprocess.run([sys.executable, "tests/run.py"])
    if ret.returncode != 0:
        logger.error("Tests failed")
        return False
    logger.info("Finished tests")

    return True

if __name__ == "__main__":
    help = False
    if any(arg.startswith("-h") or arg in ("--h", "--he", "--hel", "--help") for arg in sys.argv[1:]):
        help = True
    args = parser.parse_args()

    if args.log:
        Path("logs").mkdir(exist_ok=True)

        file_handler = logging.FileHandler("logs/ci.log", mode="w", encoding="utf-8")
        file_handler.setLevel(logging.DEBUG)
        file_formatter = logging.Formatter("%(asctime)s [%(levelname)s] %(message)s")
        file_handler.setFormatter(file_formatter)
        logger.addHandler(file_handler)

        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        console_formatter = logging.Formatter("[%(levelname)s] %(message)s")
        console_handler.setFormatter(console_formatter)
        logger.addHandler(console_handler)

    logger.debug(f"Debug: {args.debug}, Clean: {args.clean}, Build: {args.build}, Test: {args.test}, Archive: {args.archive}")
    archive_name = args.archive_name or "linked-assembly"
    logger.debug(f"Archive name: {archive_name}")

    result: bool = main(args=args)
    if not result:
        sys.exit(1)

    if args.archive:
        Path("archives").mkdir(parents=True, exist_ok=True)
        archive("dist", f"archives/{archive_name}")
