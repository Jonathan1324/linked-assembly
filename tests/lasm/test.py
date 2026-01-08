from tests.lasm.assembler import Format, Bits, Arch, arch_map, bits_map, format_map
from tests.lasm.lasm import run_lasm
from tests.lasm.nasm import run_nasm

from pathlib import Path
import logging
import shutil

logger = logging.getLogger("tests")

def test_assembler(log_dir: Path, src_dir: Path, build_dir: Path, arch: Arch, glob: str, name: str, run_function) -> list[Path]:
    outputs = []
    for asmfile in src_dir.rglob(glob):
        dst_path = build_dir / name / asmfile.relative_to(src_dir)
        dst_path.parent.mkdir(parents=True, exist_ok=True)
        
        log_path = log_dir / name / asmfile.relative_to(src_dir)
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

def write_cmp_file(cmp_file: Path, content: list[list[Path]]):
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

assemblers = {
    "lasm": run_lasm,
    "nasm": run_nasm
}

def test(dir: Path, log_dir: Path):
    build_dir = dir / "build"
    srcs_dir = dir / "srcs"

    test_dirs = [src for src in srcs_dir.iterdir() if src.is_dir()]

    for test_dir in test_dirs:
        test_name = test_dir.relative_to(srcs_dir)

        info_file = test_dir / "test.info"
        info_content = info_file.read_text(encoding="utf-8").strip()

        test_assemblers_raw = info_content.split(",")
        test_assemblers: list[str] = []
        for test_assembler_raw in test_assemblers_raw:
            if test_assembler_raw in assemblers:
                if shutil.which(test_assembler_raw):
                    test_assemblers.append(test_assembler_raw)
                else:
                    logger.warning(f"{test_assembler_raw} couldn't be found")
            else:
                logger.error(f"Unknown assembler: {test_assembler_raw}")

        if len(test_assemblers) > 2:
            logger.warning(f"More than two assemblers listed in {info_file}")
            continue

        assembler_functions = []
        for test_assembler_name in test_assemblers:
            assembler_functions.append(assemblers[test_assembler_name])

        test_build_dir = build_dir / test_name
        test_log_dir = log_dir / test_name

        assembler_outs: list[list[Path]] = []
        for assembler_name, assembler_function in zip(test_assemblers, assembler_functions):
            arch = Arch.X86 # TODO: hardcoded
            outs = test_assembler(test_log_dir, test_dir, test_build_dir, arch, "*.asm", assembler_name, assembler_function)
            assembler_outs.append(outs)

        test_cmp_file = test_log_dir / "cmp.log"
        if len(test_assemblers) > 1: # == 2
            test_cmp_file_content: list[list[str]] = []
            for as1_out, as2_out in zip(assembler_outs[0], assembler_outs[1]):
                equal = check_if_files_equal(as1_out, as2_out)
                test_cmp_file_content.append([
                    str(as1_out.relative_to(test_build_dir)),
                    "-",
                    str(as2_out.relative_to(test_build_dir)),
                    ":",
                    "equal" if equal else "different"
                ])
            write_cmp_file(test_cmp_file, test_cmp_file_content)

    return True

def clean(dir: Path, log_dir: Path):
    if log_dir.exists() and log_dir.is_dir():
        shutil.rmtree(log_dir)
    
    build_dir = dir / "build"
    if build_dir.exists() and build_dir.is_dir():
        shutil.rmtree(build_dir)
