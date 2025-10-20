from assembler.assembler import Format, Bits, Arch, arch_map, bits_map, format_map
from pathlib import Path
import logging
import subprocess
from typing import List

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

def test_nasm(log_dir: Path, src_dir: Path, build_dir: Path, archs: List[Arch], bitss: List[Bits], formats: List[Format], glob: str) -> List[Path]:
    outputs = []
    for asmfile in src_dir.rglob(glob):
        asmfile_parent = asmfile.parent.parts

        dst_path = Path(build_dir, "nasm", *asmfile_parent[3:], asmfile.name)
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        
        log_path = Path(log_dir, "nasm", *asmfile_parent[3:])
        log_path.mkdir(parents=True, exist_ok=True)

        wantError = str(asmfile).startswith("e")

        for arch in archs:
            for format in formats:
                for bits in bitss:
                    arch_str: str
                    bits_str: str
                    format_str: str
                    try:
                        arch_str = arch_map[arch]
                        bits_str = bits_map[bits]
                        format_str = format_map[format]
                    except KeyError as e:
                        raise ValueError(f"Unsupported value: {e}")
                    result, output = run_nasm(
                        src=asmfile,
                        dst=dst_path,
                        debug=True,
                        logs=log_path,
                        arch=arch,
                        bits=bits,
                        format=format
                    )
                    if result:
                        if wantError:
                            logger.warning(f"Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} successful")
                        else:
                            logger.debug(f"Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} successful")
                    else:
                        if wantError:
                            logger.debug(f"Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} failed")
                        else:
                            logger.warning(f"Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} failed")
                    outputs.append(output)
    return outputs
