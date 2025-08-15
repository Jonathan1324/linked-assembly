import subprocess
from pathlib import Path
from enum import Enum

class Format(Enum):
    BIN = 1
    ELF = 2
#   COFF = 3
#   MACHO = 4

class Bits(Enum):
    B16 = 1
    B32 = 2
    B64 = 3

class Arch(Enum):
    X86 = 1
#   ARM = 2
#   RISCV = 3

log_dir = Path("logs/tests")

def runAssembler(src: str, dst: str, debug: bool, logs: Path,
                 arch: Arch, bits: Bits, format: Format):
    cmd = ["dist/bin/assembler", src, "-o", dst]
    if (debug): cmd.append("-d")

    arch_str: str
    bits_str: str
    format_str: str

    match arch:
        case Arch.X86:
            arch_str = "x86"
            cmd.extend(["--arch", "x86"])

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

    log_path = Path(f"{logs}/{Path(src).name}-{arch_str}-{bits_str}-{format_str}.txt")
    
    with open(log_path.resolve(), "w") as f:
        subprocess.run(cmd, stdout=f, stderr=f)

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

        runAssembler(
            src=str(asmfile),
            dst=f"{dst_path}.bin",
            debug=True,
            logs=log_path.parent,
            arch=Arch.X86,
            bits=Bits.B32,
            format=Format.BIN
        )

        runAssembler(
            src=str(asmfile),
            dst=f"{dst_path}-elf.o",
            debug=True,
            logs=log_path.parent,
            arch=Arch.X86,
            bits=Bits.B32,
            format=Format.ELF
        )

    exit(0)