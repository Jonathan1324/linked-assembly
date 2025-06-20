#include "architecture.hpp"

#include <string>
#include <iostream>
#include <cinttypes>
#include <cstring>
#include <cstdint>

namespace ELF {
    constexpr unsigned char Magic[] = {0x7F, 'E', 'L', 'F'};
}

namespace MACHO {
    constexpr unsigned char Magics[][4] = {
        {0xFE, 0xED, 0xFA, 0xCE},
        {0xFE, 0xED, 0xFA, 0xCF},
        {0xCE, 0xFA, 0xED, 0xFE},
        {0xCF, 0xFA, 0xED, 0xFE},
    };
    constexpr size_t Count = sizeof(Magics) / sizeof(Magics[0]);
}

namespace COFF {

}

Format getFormat(const char buffer[])
{
    if (memcmp(buffer, ELF::Magic, 4) == 0)
        return Format::ELF;
    else
    {
        for (size_t i = 0; i < MACHO::Count; ++i) {
            if (memcmp(buffer, MACHO::Magics[i], 4) == 0)
                return Format::MACHO;
        }
    }

    //TODO: add proper detection
    uint16_t machine = (static_cast<unsigned char>(buffer[1]) << 8)
                    | static_cast<unsigned char>(buffer[0]);
    
    switch (machine) {
        case 0x014C: // x86
        case 0x8664: // x86-64
        case 0xAA64: // ARM64
        case 0x01C0: // ARM
        case 0x01C4: // ARMv7
        case 0x01F0: // PowerPC
            return Format::COFF;
        
        default:
            std::cerr << "Warning: Unknown Machine type 0x"
                      << std::hex << machine
                      << " — assuming COFF anyway"
                      << std::endl;
            return Format::COFF;
    }

    return Format::Unknown;
}