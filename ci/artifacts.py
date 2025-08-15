import logging
import os
import shutil

logger = logging.getLogger("ci")

def stage_artifacts():
    logger.debug("Staging the artifacts")

    dist_bin = "dist/bin"

    os.makedirs(dist_bin, exist_ok=True)

    if os.path.exists("build"):
        for root, dirs, files in os.walk("build"):
            for f in files:
                pass
    else:
        logger.warning("build/ folder does not exist, skipping binaries")