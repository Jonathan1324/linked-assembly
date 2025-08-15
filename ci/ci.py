from ci.build import build, clean
from ci.artifacts import stage_artifacts

from pathlib import Path
import argparse
import logging
import sys

# Logger
Path("logs").mkdir(exist_ok=True)

logger = logging.getLogger("ci")
logger.setLevel(logging.DEBUG)

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
    "--no-build",
    dest="build",
    action="store_false",
    help="Stop before building the project"
)

if __name__ == "__main__":
    help = False
    if any(arg.startswith("-h") or arg in ("--h", "--he", "--hel", "--help") for arg in sys.argv[1:]):
        help = True

    logger.debug(f"Help: {help}")

    if (not help): logger.debug("Parsing through the args")
    args = parser.parse_args()
    logger.debug(f"Debug: {args.debug}, Clean: {args.clean}, Build: {args.build}")

    if (args.clean): clean(debug=args.debug)

    if (not args.build):
        logger.debug("Stopping before building")
        sys.exit(0)

    build(debug=args.debug)

    stage_artifacts()