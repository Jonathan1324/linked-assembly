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

enum class Endianness {
    Little,
    Big
};

enum class Format {
    Binary,
    ELF,
    COFF,
    MACHO,
};

namespace LittleEndian {
    void write(std::ofstream& out, uint8_t value);
    void write(std::ofstream& out, uint16_t value);
    void write(std::ofstream& out, uint32_t value);
    void write(std::ofstream& out, uint64_t value);
    void write(std::ofstream& out, const char value[], size_t length);
    void write(std::ofstream& out, const unsigned char value[], size_t length);
}

namespace BigEndian {
    void write(std::ofstream& out, uint8_t value);
    void write(std::ofstream& out, uint16_t value);
    void write(std::ofstream& out, uint32_t value);
    void write(std::ofstream& out, uint64_t value);
    void write(std::ofstream& out, const char value[], size_t length);
    void write(std::ofstream& out, const unsigned char value[], size_t length);
}

namespace Endian {
    void write(std::ofstream& out, const char value[], int length, Endianness endianness);
    void write(std::ofstream& out, const unsigned char value[], int length, Endianness endianness);

    template<typename T>
    void write(std::vector<unsigned char>& out, T value, Endianness endianness) {
        unsigned char bytes[sizeof(T)];
        if (endianness == Endianness::Little) {
            for (size_t i = 0; i < sizeof(T); ++i)
                bytes[i] = (value >> (8 * i)) & 0xFF;
        } else {
            for (size_t i = 0; i < sizeof(T); ++i)
                bytes[sizeof(T) - 1 - i] = (value >> (8 * i)) & 0xFF;
        }
        out.insert(out.end(), bytes, bytes + sizeof(T));
    }

    template<typename T>
    void write(std::ofstream& out, T value, Endianness endianness)
    {
        if (endianness == Endianness::Little)
            LittleEndian::write(out, value);
        else
            BigEndian::write(out, value);
    }
}