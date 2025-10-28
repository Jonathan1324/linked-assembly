from pathlib import Path
import logging
import subprocess
import shutil

logger = logging.getLogger("tests")

def test(dir: Path, log_dir: Path):
    lfs = Path("dist/bin/lfs")
    src_dir = dir / "srcs"

def clean(dir: Path, log_dir: Path):
    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)
