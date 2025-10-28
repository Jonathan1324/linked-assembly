from assembler.assembler import Format, Bits, Arch, arch_map, bits_map, format_map
from pathlib import Path
import logging
import subprocess
from typing import List

logger = logging.getLogger("tests")

def run_lasm(src: Path, dst: Path, logs: Path, debug: bool,
        arch: Arch, bits: Bits, format: Format) -> tuple[bool, Path]:
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
        cmd.extend(["--bits", "16"])
    elif bits == Bits.B32:
        bits_str = "32bit"
        cmd.extend(["--bits", "32"])
    elif bits == Bits.B64:
        bits_str = "64bit"
        cmd.extend(["--bits", "64"])
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

    out = Path(f"{dst}-{arch_str}-{bits_str}-{format_str}.{ext}")
    cmd.extend(["-o", str(out)])
    log_path = Path(f"{logs}/{Path(src).name}-{arch_str}-{bits_str}-{format_str}.txt")

    with open(log_path.resolve(), "w") as f:
        result = subprocess.run(cmd, stdout=f, stderr=f)

    return (result.returncode == 0, out)
