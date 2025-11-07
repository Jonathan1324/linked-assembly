from pathlib import Path
import logging
import subprocess
import shutil

logger = logging.getLogger("tests")

def test(dir: Path, log_dir: Path):
    lfs = Path("dist/bin/lfs")
    root_dir = dir / "roots"

    roots = [p for p in root_dir.iterdir() if p.is_dir() and p.name != ".ignore"]
    for root in roots:
        for fs_type in ["fat12", "fat16", "fat32"]:
            log_path = Path(f"{log_dir}/{root.name}-{fs_type}.txt")
            log_path.parent.mkdir(parents=True, exist_ok=True)

            out_path = Path(f"{dir}/build/{root.name}/{fs_type}.img")
            out_path.parent.mkdir(parents=True, exist_ok=True)

            with open(log_path, "w") as f:
                size = "0"
                if   fs_type == "fat12": size = "1440K"
                elif fs_type == "fat16": size = "16M"
                elif fs_type == "fat32": size = "100M"
                cmd = [str(lfs), "create", fs_type, str(out_path), "--root", str(root), "--size", size]
                subprocess.run(cmd, stdout=f, stderr=f)

def clean(dir: Path, log_dir: Path):
    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)
