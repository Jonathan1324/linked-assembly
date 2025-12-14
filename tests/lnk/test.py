from pathlib import Path
import logging
import subprocess
import shutil

logger = logging.getLogger("tests")

def test(dir: Path, log_dir: Path):
    lnk = Path("dist/bin/lnk")
    bundle_dir = dir / "bundles"

    bundles = [p for p in bundle_dir.iterdir() if p.is_dir() and p.name != ".ignore"]
    for bundle in bundles:
        build_dir = Path(f"{dir}/build/{bundle.name}.o")
        build_dir.parent.mkdir(parents=True, exist_ok=True)

        log_dir = Path(f"{log_dir}/{bundle.name}.txt")
        log_dir.parent.mkdir(parents=True, exist_ok=True)

        objects = [o for o in bundle.rglob("*") if o.is_file()]

        # TODO

def clean(dir: Path, log_dir: Path):
    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)

    build_dir = dir / "build"

    if build_dir.exists() and build_dir.is_dir():
        shutil.rmtree(build_dir)
