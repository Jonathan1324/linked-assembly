import subprocess
import logging

logger = logging.getLogger("ci")

def build(debug):
    logger.info("Building the project")
    cmd = ["make"]
    if (debug): cmd.append("DEBUG=1")
    subprocess.run(cmd)

def clean(debug):
    logger.info("Cleaning")
    cmd = ["make", "clean"]
    if (debug): cmd.append("DEBUG=1")
    subprocess.run(cmd)