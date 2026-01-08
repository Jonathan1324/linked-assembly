from pathlib import Path
import logging
import shutil
import subprocess
from typing import List

logger = logging.getLogger("tests")

def run_lasmp(src: Path, dst: Path, logs: Path) -> bool:
    lasmp = Path("dist/bin/lasmp")
    cmd = [str(lasmp), str(src), "-o", dst]

    log_path = Path(f"{logs}/{Path(src).name}.txt")

    with open(log_path.resolve(), "w") as f:
        result = subprocess.run(cmd, stdout=f, stderr=f)

    return result.returncode == 0

def test(dir: Path, log_dir: Path):
    build_dir = dir / "build"

    for asmfile in (dir / "srcs").rglob("*.asm"):
        asmfile_parent = asmfile.parent.parts

        dst_path = Path(build_dir, *asmfile_parent[3:], asmfile.name)
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        
        log_path = Path(log_dir, *asmfile_parent[3:])
        log_path.mkdir(parents=True, exist_ok=True)

        wantError = asmfile.name.startswith("e")

        result = run_lasmp(
            src=asmfile,
            dst=dst_path,
            logs=log_path
        )
        if result:
            if wantError:
                logger.warning(f"{asmfile} successful")
            else:
                logger.debug(f"{asmfile} successful")
        else:
            if wantError:
                logger.debug(f"{asmfile} failed")
            else:
                logger.warning(f"{asmfile} failed")

def clean(dir: Path, log_dir: Path):
    build_dir = dir / "build"

    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)

    if build_dir.exists() and build_dir.is_dir():
        shutil.rmtree(build_dir)
