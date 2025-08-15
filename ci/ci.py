from ci.build import build, clean

from pathlib import Path
import argparse
import logging
import sys

# Logger
Path("logs").mkdir(exist_ok=True)
logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s [%(levelname)s] %(message)s",
    handlers= [
        logging.FileHandler("logs/ci.log", mode="w", encoding="utf-8"),
        logging.StreamHandler()
    ]
)

# Argument parser
parser = argparse.ArgumentParser(description="CI/CD Script")
parser.add_argument(
    "-d", "--debug",
    action="store_true",
    help="Enable debug build"
)
parser.add_argument(
    "-c", "--clean",
    action="store_true",
    help="Clean builds"
)
parser.add_argument(
    "--no-build",
    action="store_true",
    help="Stop before building the project"
)

if __name__ == "__main__":
    if any(arg.startswith("-h") or arg in ("--h", "--he", "--hel", "--help") for arg in sys.argv[1:]):
        logging.info("Showing help message")

    args = parser.parse_args()
    logging.info(f"Debug: {args.debug}, Clean: {args.clean}, Build: {not args.no_build}")

    if (args.clean): clean(debug=args.debug)

    if (args.no_build): sys.exit(0)

    build(debug=args.debug)
