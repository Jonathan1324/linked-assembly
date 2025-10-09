from enum import Enum

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