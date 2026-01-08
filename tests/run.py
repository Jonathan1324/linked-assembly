from pathlib import Path
import logging
import os
import sys

import assembler.test as assembler
import lasmp.test as lasmp

import lnk.test as lnk

import buildtool.test as buildtool

import lbf.test as lbf

import lfs.test as lfs

# Paths
dir = Path("tests")
log_dir = Path("logs")
logs = log_dir / "tests.log"

# Logger
logger = logging.getLogger("tests")
logger.setLevel(logging.DEBUG)

os.makedirs("logs", exist_ok=True)

file_handler = logging.FileHandler(str(logs), mode="w", encoding="utf-8")
file_handler.setLevel(logging.DEBUG)
file_formatter = logging.Formatter("%(asctime)s [%(levelname)s] %(message)s")
file_handler.setFormatter(file_formatter)
logger.addHandler(file_handler)

console_handler = logging.StreamHandler()
console_handler.setLevel(logging.INFO)
console_formatter = logging.Formatter("[%(levelname)s] %(message)s")
console_handler.setFormatter(console_formatter)
logger.addHandler(console_handler)

def main():
    return

    assembler_dir = dir / "assembler"
    assembler_log_dir = log_dir / "assembler"

    lasmp_dir = dir / "lasmp"
    lasmp_log_dir = log_dir / "lasmp"

    lnk_dir = dir / "lnk"
    lnk_log_dir = log_dir / "lnk"

    buildtool_dir = dir / "buildtool"
    buildtool_log_dir = log_dir / "buildtool"

    lbf_dir = dir / "lbf"
    lbf_log_dir = log_dir / "lbf"

    lfs_dir = dir / "lfs"
    lfs_log_dir = log_dir / "lfs"
    
    if "-c" in sys.argv[1:]:
        assembler.clean(assembler_dir, assembler_log_dir)
        lasmp.clean(lasmp_dir, lasmp_log_dir)

        lnk.clean(lnk_dir, lnk_log_dir)

        buildtool.clean(buildtool_dir, buildtool_log_dir)

        lbf.clean(lbf_dir, lbf_log_dir)

        lfs.clean(lfs_dir, lfs_log_dir)

        logs.unlink(missing_ok=True)
    else:
        assembler_log_dir.mkdir(parents=True, exist_ok=True)
        lasmp_log_dir.mkdir(parents=True, exist_ok=True)

        lnk_log_dir.mkdir(parents=True, exist_ok=True)

        buildtool_log_dir.mkdir(parents=True, exist_ok=True)

        lbf_log_dir.mkdir(parents=True, exist_ok=True)

        lfs_log_dir.mkdir(parents=True, exist_ok=True)

        assembler.test(assembler_dir, assembler_log_dir)
        lasmp.test(lasmp_dir, lasmp_log_dir)

        lnk.test(lnk_dir, lnk_log_dir)
        
        buildtool.test(buildtool_dir, buildtool_log_dir)

        lbf.test(lbf_dir, lbf_log_dir)

        lfs.test(lfs_dir, lfs_log_dir)

if __name__ == "__main__":
    main()
    exit(0)
