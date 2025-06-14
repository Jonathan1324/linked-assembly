#include "architecture.hpp"

namespace LittleEndian {
    void write(std::ofstream& out, uint8_t value)
    {
        out.write(reinterpret_cast<const char*>(&value), 1);
    }

    void write(std::ofstream& out, uint16_t value)
    {
        unsigned char bytes[2];
        bytes[0] = value & 0xFF;         // niedrigstes Byte zuerst
        bytes[1] = (value >> 8) & 0xFF;
        out.write(reinterpret_cast<const char*>(bytes), 2);
    }

    void write(std::ofstream& out, uint32_t value)
    {
        unsigned char bytes[4];
        bytes[0] = value & 0xFF;         // niedrigstes Byte zuerst
        bytes[1] = (value >> 8) & 0xFF;
        bytes[2] = (value >> 16) & 0xFF;
        bytes[3] = (value >> 24) & 0xFF; // höchstes Byte zuletzt
        out.write(reinterpret_cast<const char*>(bytes), 4);
    }

    void write(std::ofstream& out, uint64_t value)
    {
        unsigned char bytes[8];
        bytes[0] = value & 0xFF;
        bytes[1] = (value >> 8) & 0xFF;
        bytes[2] = (value >> 16) & 0xFF;
        bytes[3] = (value >> 24) & 0xFF;
        bytes[4] = (value >> 32) & 0xFF;
        bytes[5] = (value >> 40) & 0xFF;
        bytes[6] = (value >> 48) & 0xFF;
        bytes[7] = (value >> 56) & 0xFF;
        out.write(reinterpret_cast<const char*>(bytes), 8);
    }

    void write(std::ofstream& out, const char value[], size_t length)
    {
        out.write(reinterpret_cast<const char*>(value), length);
    }

    void write(std::ofstream& out, const unsigned char value[], size_t length)
    {
        out.write(reinterpret_cast<const char*>(value), length);
    }
}

namespace BigEndian {
    void write(std::ofstream& out, uint8_t value)
    {
        out.write(reinterpret_cast<const char*>(&value), 1);
    }

    void write(std::ofstream& out, uint16_t value)
    {
        unsigned char bytes[2];
        bytes[0] = (value >> 8) & 0xFF;  // höchstes Byte zuerst
        bytes[1] = value & 0xFF;         // niedrigstes Byte zuletzt
        out.write(reinterpret_cast<const char*>(bytes), 2);
    }

    void write(std::ofstream& out, uint32_t value)
    {
        unsigned char bytes[4];
        bytes[0] = (value >> 24) & 0xFF; // höchstes Byte zuerst
        bytes[1] = (value >> 16) & 0xFF;
        bytes[2] = (value >> 8) & 0xFF;
        bytes[3] = value & 0xFF;         // niedrigstes Byte zuletzt
        out.write(reinterpret_cast<const char*>(bytes), 4);
    }

    void write(std::ofstream& out, uint64_t value)
    {
        unsigned char bytes[8];
        bytes[0] = (value >> 56) & 0xFF; // höchstes Byte zuerst
        bytes[1] = (value >> 48) & 0xFF;
        bytes[2] = (value >> 40) & 0xFF;
        bytes[3] = (value >> 32) & 0xFF;
        bytes[4] = (value >> 24) & 0xFF;
        bytes[5] = (value >> 16) & 0xFF;
        bytes[6] = (value >> 8) & 0xFF;
        bytes[7] = value & 0xFF;         // niedrigstes Byte zuletzt
        out.write(reinterpret_cast<const char*>(bytes), 8);
    }

    void write(std::ofstream& out, const char value[], size_t length)
    {
        out.write(reinterpret_cast<const char*>(value), length);
    }

    void write(std::ofstream& out, const unsigned char value[], size_t length)
    {
        out.write(reinterpret_cast<const char*>(value), length);
    }
}

namespace Endian {
    void write(std::ofstream& out, const char value[], int length, Endianness endianness)
    {
        if (endianness == Endianness::Little)
            LittleEndian::write(out, value, (size_t)length);
        else
            BigEndian::write(out, value, (size_t)length);
    }

    void write(std::ofstream& out, const unsigned char value[], int length, Endianness endianness)
    {
        if (endianness == Endianness::Little)
            LittleEndian::write(out, value, (size_t)length);
        else
            BigEndian::write(out, value, (size_t)length);
    }
}