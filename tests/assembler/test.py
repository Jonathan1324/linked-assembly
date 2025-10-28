from assembler.assembler import Format, Bits, Arch, arch_map, bits_map, format_map
from assembler.lasm import run_lasm
from assembler.nasm import run_nasm
from pathlib import Path
import logging
import shutil
from typing import List

logger = logging.getLogger("tests")

def test_assembler(log_dir: Path, src_dir: Path, build_dir: Path, arch: Arch, glob: str, name: str, run_function) -> List[Path]:
    outputs = []
    for asmfile in src_dir.rglob(glob):
        asmfile_parent = asmfile.parent.parts

        dst_path = Path(build_dir, name, *asmfile_parent[3:], asmfile.name)
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        
        log_path = Path(log_dir, name, *asmfile_parent[3:])
        log_path.mkdir(parents=True, exist_ok=True)

        formats = []
        bitss = []
        expects = None

        with open(asmfile) as f:
            for line in [f.readline() for _ in range(3)]:
                line = line.strip().lower()

                if "formats:" in line:
                    content = line.split("formats:", 1)[1].strip()
                    for x in content.split(","):
                        x = x.strip()
                        if x == "bin":
                            formats.append(Format.BIN)
                        elif x == "elf":
                            formats.append(Format.ELF)
                        elif x == "coff":
                            formats.append(Format.COFF)
                        elif x == "macho":
                            formats.append(Format.MACHO)
                        else:
                            logger.error(f"{asmfile} has unknown format: '{x}'")
                
                elif "bits:" in line:
                    content = line.split("bits:", 1)[1].strip()
                    for x in content.split(","):
                        x = x.strip()
                        if x == "16":
                            bitss.append(Bits.B16)
                        elif x == "32":
                            bitss.append(Bits.B32)
                        elif x == "64":
                            bitss.append(Bits.B64)
                        else:
                            logger.error(f"{asmfile} has unknown bits: '{x}'")

                elif "expect:" in line:
                    content = line.split("expect:", 1)[1].strip()
                    if content == "success":
                        expects = True
                    elif content == "error":
                        expects = False
                    else:
                        logger.error(f"{asmfile} has unknown expectation: '{content}'")

        if not formats:
            logger.warning(f"{asmfile}: no formats defined")

        if not bitss:
            logger.warning(f"{asmfile}: no bits defined")

        if expects is None:
            logger.warning(f"{asmfile}: no expectation defined - defaulting to success")
            expects = True  # Default

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
                result, output = run_function(
                    src=asmfile,
                    dst=dst_path,
                    debug=True,
                    logs=log_path,
                    arch=arch,
                    bits=bits,
                    format=format
                )
                if result:
                    if expects:
                        logger.debug(f"({name}) Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} successful")
                    else:
                        logger.warning(f"({name}) Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} successful")
                else:
                    if expects:
                        logger.warning(f"({name}) Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} failed")
                    else:
                        logger.debug(f"({name}) Arch: {arch_str}, Bits: {bits_str}, Format: {format_str}; {asmfile} failed")
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

    test_assembler(log_dir / "x86", dir / "x86", build_dir / "x86",
                  Arch.X86,
                  "*.asm", "lasm", run_lasm)
    
    nasm_log_dir = log_dir / "nasm"
    nasm_source_dir = dir / "nasm"
    nasm_build_dir = build_dir / "nasm"
    nasm_cmp_file = nasm_log_dir / "cmp.log"
    nasm_lasm_outs: List[Path] = test_assembler(nasm_log_dir, nasm_source_dir, nasm_build_dir,
                                                Arch.X86,
                                                "*.asm", "lasm", run_lasm)
    nasm_nasm_outs: List[Path] = test_assembler(nasm_log_dir, nasm_source_dir, nasm_build_dir,
                                                Arch.X86,
                                                "*.asm", "nasm", run_nasm)
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
