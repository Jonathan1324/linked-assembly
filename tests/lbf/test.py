from pathlib import Path
import logging
import subprocess
import shutil

logger = logging.getLogger("tests")

def test(dir: Path, log_dir: Path):
    lbf = Path("dist/bin/lbf")
    src_dir = dir / "srcs"

    for file in src_dir.rglob("*.bf"):
        file_parts = file.parts
        name = Path(*file_parts[3:])
        log_path = Path(f"{log_dir}/{name}.txt")
        log_path.parent.mkdir(parents=True, exist_ok=True)

        cmd = [str(lbf), str(file)]
        with open(log_path, "w") as f:
            subprocess.run(cmd, stdout=f, stderr=f)

def clean(dir: Path, log_dir: Path):
    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)
