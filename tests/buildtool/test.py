from pathlib import Path
from contextlib import contextmanager
import logging
import os
import subprocess
import shutil

logger = logging.getLogger("tests")

@contextmanager
def change_dir(path: Path):
    prev = Path.cwd()
    os.chdir(path)
    try:
        yield
    finally:
        os.chdir(prev)

def test(dir: Path, log_dir: Path):
    env_dir = dir / "envs"
    buildtool_rel = Path("dist/bin/lbt")

    buildtool = str(buildtool_rel.resolve())
    envs = [p for p in env_dir.iterdir() if p.is_dir() and p.name != ".ignore"]
    for env in envs:
        log_path = Path(f"{log_dir}/{env.name}.txt")
        log_path.parent.mkdir(parents=True, exist_ok=True)
        with open(log_path, "w") as f:
            cmd = [buildtool]
            with change_dir(env):
                subprocess.run(cmd, stdout=f, stderr=f)

def clean(dir: Path, log_dir: Path):
    env_dir = dir / "envs"
    
    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)

    envs = [p for p in env_dir.iterdir() if p.is_dir() and p.name != ".ignore"]
    for env in envs:
        build_dir = env / "build"
        if build_dir.exists() and build_dir.is_dir():
            shutil.rmtree(build_dir)
