from pathlib import Path
from enum import Enum
import subprocess
import logging
import buildtool.test as buildtool

# Logger
logger = logging.getLogger("tests")
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

def runAssembler(src: str, dst: str, debug: bool, logs: Path,
                 arch: Arch, bits: Bits, format: Format) -> bool:
    cmd = ["dist/bin/lasm", src]
    if (debug): cmd.append("-d")

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

def testAssembler(src_dir: Path, build_dir: Path, log_dir: Path):
    for asmfile in src_dir.rglob("*.asm"):
        dst_parts = (build_dir / asmfile.parent).parts
        dst_path = Path(*dst_parts, asmfile.name)
        dst_path.parent.mkdir(parents=True, exist_ok=True)

        log_parts = (log_dir / asmfile.parent).parts
        log_path = Path(*log_parts[:2], *log_parts[3:], asmfile.name)
        log_path.parent.mkdir(parents=True, exist_ok=True)

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


if __name__ == "__main__":
    assembler_dir = Path("tests/assembler")
    assembler_log_dir = Path("logs/assembler/tests-verbose")
    assembler_build_dir = assembler_dir / "build"
    assembler_src_dir = assembler_dir / "srcs"
    assembler_build_dir.mkdir(parents=True, exist_ok=True)
    assembler_log_dir.mkdir(parents=True, exist_ok=True)

    testAssembler(assembler_src_dir, assembler_build_dir, assembler_log_dir)

    buildtool_dir = Path("tests/buildtool")
    buildtool_log_dir = Path("logs/buildtool")
    buildtool_log_dir.mkdir(parents=True, exist_ok=True)
    buildtool.test(buildtool_dir, buildtool_log_dir)

    exit(0)
