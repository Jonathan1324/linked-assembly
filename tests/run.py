from pathlib import Path
from enum import Enum
import subprocess
import logging

# Logger
logger = logging.getLogger("ci")
logger.setLevel(logging.DEBUG)

file_handler = logging.FileHandler("logs/tests.log", mode="w", encoding="utf-8")
file_handler.setLevel(logging.DEBUG)
file_formatter = logging.Formatter("%(asctime)s [%(levelname)s] %(message)s")
file_handler.setFormatter(file_formatter)
logger.addHandler(file_handler)

console_handler = logging.StreamHandler()
console_handler.setLevel(logging.INFO)
console_formatter = logging.Formatter("[%(levelname)s] %(message)s")
console_handler.setFormatter(console_formatter)
logger.addHandler(console_handler)

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

log_dir = Path("logs/tests-verbose")

def runAssembler(src: str, dst: str, debug: bool, logs: Path,
                 arch: Arch, bits: Bits, format: Format) -> bool:
    cmd = ["dist/bin/assembler", src]
    if (debug): cmd.append("-d")

    arch_str: str
    bits_str: str
    format_str: str

    match arch:
        case Arch.X86:
            arch_str = "x86"
            cmd.extend(["--arch", "x86"])
        case Arch.ARM:
            arch_str = "arm"
            cmd.extend(["--arch", "arm"])
        case Arch.RISCV:
            arch_str = "riscv"
            cmd.extend(["--arch", "riscv"])

    match bits:
        case Bits.B16:
            bits_str = "16bit"
            cmd.append("-m16")
        case Bits.B32:
            bits_str = "32bit"
            cmd.append("-m32")
        case Bits.B64:
            bits_str = "64bit"
            cmd.append("-m64")

    match format:
        case Format.BIN:
            format_str = "bin"
            cmd.extend(["--format", "bin"])
        case Format.ELF:
            format_str = "elf"
            cmd.extend(["--format", "elf"])
        case Format.COFF:
            format_str = "coff"
            cmd.extend(["--format", "coff"])
        case Format.MACHO:
            format_str = "macho"
            cmd.extend(["--format", "macho"])

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

if __name__ == "__main__":
    build_dir = Path("tests/build")
    build_dir.mkdir(parents=True, exist_ok=True)
    log_dir.mkdir(parents=True, exist_ok=True)

    src_path = Path("tests/srcs")

    for asmfile in src_path.rglob("*.asm"):
        dst_parts = (Path(build_dir) / asmfile.parent).parts
        dst_path = Path(*dst_parts[:2], *dst_parts[4:], asmfile.name)
        dst_path.parent.mkdir(parents=True, exist_ok=True)

        log_parts = (Path(log_dir) / asmfile.parent).parts
        log_path = Path(*log_parts[:2], *log_parts[4:], asmfile.name)
        log_path.parent.mkdir(parents=True, exist_ok=True)

        for arch in [Arch.X86]:
            for bits in [Bits.B16, Bits.B32, Bits.B64]:
                for format in [Format.BIN, Format.ELF]:
                    arch_str: str
                    bits_str: str
                    format_str: str
                    match arch:
                        case Arch.X86: arch_str = "x86"
                        case Arch.ARM: arch_str = "arm"
                        case Arch.RISCV: arch_str = "riscv"
                    match bits:
                        case Bits.B16: bits_str = "16bit"
                        case Bits.B32: bits_str = "32bit"
                        case Bits.B64: bits_str = "64bit"
                    match format:
                        case Format.BIN: format_str = "bin"
                        case Format.ELF: format_str = "elf"
                        case Format.COFF: format_str = "coff"
                        case Format.MACHO: format_str = "macho"
                            
                    result = runAssembler(
                        src=str(asmfile),
                        dst=f"{dst_path}",
                        debug=True,
                        logs=log_path.parent,
                        arch=arch,
                        bits=bits,
                        format=format
                    )
                    if (result):
                        logger.debug(f"Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} successful")
                    else:
                        logger.warning(f"Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} failed")

    exit(0)