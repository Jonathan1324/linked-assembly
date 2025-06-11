#pragma once

enum class Architecture {
    x86,
    ARM,
    RISC_V
};

enum class BitMode {
    Bits16,
    Bits32,
    Bits64
};

enum class Format {
    ELF,
    COFF,
    MACHO,
};