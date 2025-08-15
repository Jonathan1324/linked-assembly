import subprocess
import logging

logger = logging.getLogger("ci")

def build(debug: bool) -> bool:
    logger.info("Building the project")
    cmd = ["make"]
    if (debug): cmd.append("DEBUG=1")
    result = subprocess.run(cmd)
    return result.returncode == 0

def clean(debug: bool) -> bool:
    logger.info("Cleaning")
    cmd = ["make", "clean"]
    if (debug): cmd.append("DEBUG=1")
    result = subprocess.run(cmd)
    return result.returncode == 0
