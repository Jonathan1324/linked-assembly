from pathlib import Path
import logging

logger = logging.getLogger("ci")

def test(dir: Path, log_dir: Path):
    logger.info("Skipping buildtool tests because they aren't implemented yet")
