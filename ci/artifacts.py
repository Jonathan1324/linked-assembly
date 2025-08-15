import logging
from pathlib import Path
import shutil

logger = logging.getLogger("ci")

def stage_artifacts(debug: bool) -> bool:
    logger.debug("Staging the artifacts")

    dist_bin = Path("dist/bin")
    dist_bin.mkdir(parents=True, exist_ok=True)

    build_type = debug if debug else "release"
    binaries = Path(f"build/{build_type}/binaries.txt")
    
    if not binaries.exists():
        logger.error(f"{binaries} does not exist")
        return False
    
    with binaries.open() as f:
        for line in f:
            bin_name = line.strip()
            if not bin_name: continue
            binary = Path(bin_name)
            if binary.exists():
                dst = dist_bin / binary.name
                shutil.copy2(binary, dst)
                logger.debug(f"Copied {binary} to {dst}")
            else:
                logger.warning(f"{binary} does not exist")
    
    logger.info(f"Staged {len(list(dist_bin.iterdir()))} binaries in dist/bin")

    binaries.unlink()
    logger.debug("Deleted {binaries}")

    return True