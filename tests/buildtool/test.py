from pathlib import Path
from contextlib import contextmanager
import logging
import os
import subprocess

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
    buildtool_rel = Path("dist/bin/buildtool")

    buildtool = str(buildtool_rel.resolve())
    envs = [p for p in env_dir.iterdir() if p.is_dir() and p.name != ".ignore"]
    for env in envs:
        log_path = Path(f"{log_dir}/envs/{env.name}.txt")
        log_path.parent.mkdir(parents=True, exist_ok=True)
        with open(log_path, "w") as f:
            cmd = [buildtool]
            with change_dir(env):
                subprocess.run(cmd, stdout=f, stderr=f)