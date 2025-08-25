from pathlib import Path
from enum import Enum
import logging
import shutil
import subprocess

logger = logging.getLogger("tests")

class Format(Enum):
    BIN = 1
    ELF = 2
    COFF = 3
    MACHO = 4

class Bits(Enum):
    B16 = 1
    B32 = 2
    B64 = 3

class Arch(Enum):
    X86 = 1
    ARM = 2
    RISCV = 3

arch_map = {
    Arch.X86: "x86",
    Arch.ARM: "arm",
    Arch.RISCV: "riscv"
}

bits_map = {
    Bits.B16: "16bit",
    Bits.B32: "32bit",
    Bits.B64: "64bit"
}

format_map = {
    Format.BIN: "bin",
    Format.ELF: "elf",
    Format.COFF: "coff",
    Format.MACHO: "macho"
}

def run(src: Path, dst: Path, logs: Path, debug: bool,
        arch: Arch, bits: Bits, format: Format) -> bool:
    assembler = Path("dist/bin/lasm")
    cmd = [str(assembler), str(src)]
    if debug: cmd.append("-d")

    arch_str: str
    bits_str: str
    format_str: str

    if arch == Arch.X86:
        arch_str = "x86"
        cmd.extend(["--arch", "x86"])
    elif arch == Arch.ARM:
        arch_str = "arm"
        cmd.extend(["--arch", "arm"])
    elif arch == Arch.RISCV:
        arch_str = "riscv"
        cmd.extend(["--arch", "riscv"])
    else:
        raise ValueError(f"Unsupported architecture: {arch}")

    if bits == Bits.B16:
        bits_str = "16bit"
        cmd.append("-m16")
    elif bits == Bits.B32:
        bits_str = "32bit"
        cmd.append("-m32")
    elif bits == Bits.B64:
        bits_str = "64bit"
        cmd.append("-m64")
    else:
        raise ValueError(f"Unsupported bit size: {bits}")

    if format == Format.BIN:
        format_str = "bin"
        cmd.extend(["--format", "bin"])
    elif format == Format.ELF:
        format_str = "elf"
        cmd.extend(["--format", "elf"])
    elif format == Format.COFF:
        format_str = "coff"
        cmd.extend(["--format", "coff"])
    elif format == Format.MACHO:
        format_str = "macho"
        cmd.extend(["--format", "macho"])
    else:
        raise ValueError(f"Unsupported format: {format}")
    
    ext: str
    if format == Format.BIN:
        ext = "bin"
    else:
        ext = "o"

    cmd.extend(["-o", f"{dst}-{arch_str}-{bits_str}-{format_str}.{ext}"])
    log_path = Path(f"{logs}/{Path(src).name}-{arch_str}-{bits_str}-{format_str}.txt")

    with open(log_path.resolve(), "w") as f:
        result = subprocess.run(cmd, stdout=f, stderr=f)

    return result.returncode == 0

def test(dir: Path, log_dir: Path):
    build_dir = dir / "build"
    src_dir = dir / "srcs"

    for asmfile in src_dir.rglob("*.asm"):
        asmfile_parent = asmfile.parent.parts

        dst_path = Path(build_dir, *asmfile_parent[3:], asmfile.name)
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        
        log_path = Path(log_dir, "srcs", *asmfile_parent[3:])
        log_path.mkdir(parents=True, exist_ok=True)

        for arch in [Arch.X86]:
            for bits in [Bits.B16, Bits.B32, Bits.B64]:
                for format in [Format.BIN, Format.ELF]:
                    arch_str: str
                    bits_str: str
                    format_str: str
                    try:
                        arch_str = arch_map[arch]
                        bits_str = bits_map[bits]
                        format_str = format_map[format]
                    except KeyError as e:
                        raise ValueError(f"Unsupported value: {e}")
                    result = run(
                        src=asmfile,
                        dst=dst_path,
                        debug=True,
                        logs=log_path,
                        arch=arch,
                        bits=bits,
                        format=format
                    )
                    if result:
                        logger.debug(f"Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} successful")
                    else:
                        logger.warning(f"Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} failed")



def clean(dir: Path, log_dir: Path):
    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)
    
    build_dir = dir / "build"
    if build_dir.exists() and build_dir.is_dir():
        shutil.rmtree(build_dir)