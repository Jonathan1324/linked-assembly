#pragma once

enum class Format {
    UNKNOWN,
    ELF,
    COFF,
    MACHO,
};

Format getFormat(const char buffer[]);