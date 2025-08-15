import subprocess
from pathlib import Path

if __name__ == "__main__":
    build_bin = Path("tests/build")
    build_bin.mkdir(parents=True, exist_ok=True)

    exit(0)