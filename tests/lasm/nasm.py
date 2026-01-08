from tests.lasm.assembler import Format, Bits, Arch, arch_map, bits_map, format_map
from pathlib import Path
import logging
import subprocess

logger = logging.getLogger("tests")

def run_nasm(src: Path, dst: Path, logs: Path, debug: bool,
        arch: Arch, bits: Bits, format: Format) -> tuple[bool, Path]:
    cmd = ["nasm", str(src)]
    # TODO: debug

    bits_str: str
    format_str: str

    if arch != Arch.X86:
        raise ValueError(f"Unsupported architecture: {arch}")
    
    if bits == Bits.B16:
        bits_str = "16bit"
    elif bits == Bits.B32:
        bits_str = "32bit"
    elif bits == Bits.B64:
        bits_str = "64bit"
    else:
        raise ValueError(f"Unsupported bit size: {bits}")
    
    if format == Format.BIN:
        format_str = "bin"
        cmd.extend(["-f", "bin"])
    elif format == Format.ELF:
        format_str = "elf"
        if bits == Bits.B64:
            cmd.extend(["-f", "elf64"])
        else:
            cmd.extend(["-f", "elf32"])
    elif format == Format.COFF:
        format_str = "coff"
        if bits == Bits.B64:
            cmd.extend(["-f", "win64"])
        else:
            cmd.extend(["-f", "win32"])
    elif format == Format.MACHO:
        format_str = "macho"
        if bits == Bits.B64:
            cmd.extend(["-f", "macho64"])
        else:
            cmd.extend(["-f", "macho32"])
    else:
        raise ValueError(f"Unsupported format: {format}")

    ext: str
    if format == Format.BIN:
        ext = "bin"
    else:
        ext = "o"

    out = Path(f"{dst}-x86-{bits_str}-{format_str}.{ext}")
    cmd.extend(["-o", str(out)])
    log_path = Path(f"{logs}/{Path(src).name}-x86-{bits_str}-{format_str}.txt")

    with open(log_path.resolve(), "w") as f:
        result = subprocess.run(cmd, stdout=f, stderr=f)

    return (result.returncode == 0, out)
