import subprocess
import logging
from ci.os import OS, ARCH, getOS, getArch

logger = logging.getLogger("ci")

def build(debug: bool, os: OS, arch: ARCH, tools: list[str]) -> bool:
    logger.info("Building the project")
    cmd = ["make"]

    if (debug): cmd.append("DEBUG=1")

    cmd.append(f"OS_NAME={getOS(os)}")
    cmd.append(f"ARCH={getArch(arch)}")

    if (not tools): cmd.append("all_tools")
    else:           cmd.extend(tools)

    result = subprocess.run(cmd)
    return result.returncode == 0

def clean(debug: bool, os: OS, arch: ARCH) -> bool:
    logger.info("Cleaning")
    cmd = ["make", "clean"]
    if (debug): cmd.append("DEBUG=1")
    cmd.append(f"OS_NAME={getOS(os)}")
    cmd.append(f"ARCH={getArch(arch)}")
    result = subprocess.run(cmd)
    return result.returncode == 0
