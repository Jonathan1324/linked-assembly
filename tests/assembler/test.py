from assembler.assembler import Format, Bits, Arch, arch_map, bits_map, format_map
from assembler.nasm import test_nasm
from pathlib import Path
import logging
import shutil
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

    out = Path(f"{dst}-{arch_str}-{bits_str}-{format_str}.{ext}")
    cmd.extend(["-o", str(out)])
    log_path = Path(f"{logs}/{Path(src).name}-{arch_str}-{bits_str}-{format_str}.txt")

    with open(log_path.resolve(), "w") as f:
        result = subprocess.run(cmd, stdout=f, stderr=f)

    return (result.returncode == 0, out)

def test_lasm(log_dir: Path, src_dir: Path, build_dir: Path, archs: List[Arch], bitss: List[Bits], formats: List[Format]) -> List[Path]:
    outputs = []
    for asmfile in src_dir.rglob("*.asm"):
        asmfile_parent = asmfile.parent.parts

        dst_path = Path(build_dir, "lasm", *asmfile_parent[3:], asmfile.name)
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        
        log_path = Path(log_dir, "lasm", *asmfile_parent[3:])
        log_path.mkdir(parents=True, exist_ok=True)

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
                    result, output = run_lasm(
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
                    outputs.append(output)
    return outputs

def check_if_files_equal(p1: Path, p2: Path, blocksize: int = 8192) -> bool:
    if not (p1.exists() and p2.exists):
        return p1.exists() == p2.exists()
    if p1.stat().st_size != p2.stat().st_size:
        return False
    
    with p1.open('rb') as f1, p2.open('rb') as f2:
        while True:
            b1 = f1.read(blocksize)
            b2 = f2.read(blocksize)
            if b1 != b2:
                return False
            if not b1:
                return True

def write_cmp_file(cmp_file: Path, content: List[List[Path]]):
    cmp_file.write_text("")

    num_cols = max(len(content) for cmp in content)
    col_widths = [0] * num_cols
    for cmp in content:
        for i, cell, in enumerate(cmp):
            col_widths[i] = max(col_widths[i], len(cell))
    
    with cmp_file.open('a', encoding="utf-8") as f:
        for cmp in content:
            for i, cell in enumerate(cmp):
                f.write(cell.ljust(col_widths[i]))
                if i < len(cmp) - 1:
                    f.write("  ")
            f.write("\n")

def test(dir: Path, log_dir: Path):
    build_dir = dir / "build"

    test_lasm(log_dir / "srcs", dir / "srcs", build_dir / "srcs",
              [Arch.X86], [Bits.B16, Bits.B32, Bits.B64], [Format.BIN, Format.ELF])
    
    nasm_log_dir = log_dir / "nasm"
    nasm_source_dir = dir / "nasm"
    nasm_build_dir = build_dir / "nasm"
    nasm_cmp_file = nasm_log_dir / "cmp.log"
    nasm_lasm_outs: List[Path] = test_lasm(nasm_log_dir, nasm_source_dir, nasm_build_dir,
                                           [Arch.X86], [Bits.B16, Bits.B32, Bits.B64], [Format.BIN, Format.ELF])
    nasm_nasm_outs: List[Path] = test_nasm(nasm_log_dir, nasm_source_dir, nasm_build_dir,
                                           [Arch.X86], [Bits.B16, Bits.B32, Bits.B64], [Format.BIN, Format.ELF])
    nasm_cmp_file_content: List[List[str]] = []
    for nasm_lasm_out, nasm_nasm_out in zip(nasm_lasm_outs, nasm_nasm_outs):
        equal = check_if_files_equal(nasm_lasm_out, nasm_nasm_out)
        nasm_cmp_file_content.append([
            str(nasm_lasm_out).removeprefix("tests/assembler/build/nasm/"),
            "-",
            str(nasm_nasm_out).removeprefix("tests/assembler/build/nasm/"),
            ":",
            'equal' if equal else 'different'
        ])
    write_cmp_file(nasm_cmp_file, nasm_cmp_file_content)


def clean(dir: Path, log_dir: Path):
    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)
    
    build_dir = dir / "build"
    if build_dir.exists() and build_dir.is_dir():
        shutil.rmtree(build_dir)
