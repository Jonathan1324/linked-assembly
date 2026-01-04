import logging
from pathlib import Path
import shutil
import os
import sys
import stat

logger = logging.getLogger("ci")

def stage(debug: bool) -> bool:
    logger.debug("Staging the artifacts")

    dist_dir = Path("dist")

    dist_bin = Path(f"{dist_dir}/bin")
    dist_bin.mkdir(parents=True, exist_ok=True)
    third_party_license_file = Path(f"{dist_dir}/THIRD_PARTY_LICENSES.txt")

    build_type = "debug" if debug else "release"
    binaries = Path(f"build/{build_type}/binaries.txt")
    licenses = Path(f"build/{build_type}/licenses.txt")

    if licenses.exists():
        with third_party_license_file.open("w", encoding="utf-8") as out_f:
            for line in licenses.read_text(encoding="utf-8").splitlines():
                license_path = Path(line.strip())
                if license_path.is_file():
                    content = license_path.read_text(encoding="utf-8")
                    out_f.write(content)
                    out_f.write("\n\n")
                    logger.debug(f"Copied content of {license_path} to {third_party_license_file}")
                else:
                    logger.warning(f"{license_path} does not exist")

        licenses.unlink()
        logger.debug(f"Deleted {licenses}")
    
    if not binaries.exists():
        logger.warning(f"{binaries} does not exist")
        return True
    
    with binaries.open() as f:
        for line in f:
            bin_name = line.strip()
            if not bin_name: continue
            binary = Path(bin_name)
            if binary.exists():
                dst = dist_bin / binary.name
                shutil.copy2(binary, dst)
                logger.debug(f"Copied {binary} to {dst}")
                if sys.platform != "win32":
                    st = os.stat(dst)
                    os.chmod(dst, st.st_mode | stat.S_IEXEC)
                    logger.debug(f"Set executable permission for {dst}")
            else:
                logger.warning(f"{binary} does not exist")
    
    logger.info(f"Staged {len(list(dist_bin.iterdir()))} binaries in dist/bin")

    binaries.unlink()
    logger.debug(f"Deleted {binaries}")

    return True
