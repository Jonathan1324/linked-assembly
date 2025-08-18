import subprocess
import logging

logger = logging.getLogger("ci")

def build(debug: bool, os: str, arch: str) -> bool:
    logger.info("Building the project")
    cmd = ["make"]
    if (debug): cmd.append("DEBUG=1")
    cmd.append(f"OS_NAME={os}")
    cmd.append(f"ARCH={arch}")
    result = subprocess.run(cmd)
    return result.returncode == 0

def clean(debug: bool, os: str, arch: str) -> bool:
    logger.info("Cleaning")
    cmd = ["make", "clean"]
    if (debug): cmd.append("DEBUG=1")
    cmd.append(f"OS_NAME={os}")
    cmd.append(f"ARCH={arch}")
    result = subprocess.run(cmd)
    return result.returncode == 0
