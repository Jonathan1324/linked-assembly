#pragma once

#include <cstdint>
#include <fstream>
#include <sstream>
#include <vector>

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
    Unknown,
    Binary,
    ELF,
    COFF,
    MACHO,
};