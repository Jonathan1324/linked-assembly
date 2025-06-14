#include "mach-o.hpp"

#include "../../util/buffer.hpp"
#include <cstring>

namespace MACHO {
    Header createHeader(BitMode bits, Architecture arch, Endianness endianness)
    {
        Header header;

        switch (bits)
        {
            case BitMode::Bits16:
            case BitMode::Bits32:
                if (endianness == Endianness::Little)
                    std::memcpy(header.magic, Magic::Cigam32, 4);
                else
                    std::memcpy(header.magic, Magic::Magic32, 4);
                break;
            case BitMode::Bits64:
                if (endianness == Endianness::Little)
                    std::memcpy(header.magic, Magic::Cigam64, 4);
                else
                    std::memcpy(header.magic, Magic::Magic64, 4);
                break;
            default:
                //TODO
                if (endianness == Endianness::Little)
                    std::memcpy(header.magic, Magic::Cigam64, 4);
                else
                    std::memcpy(header.magic, Magic::Magic64, 4);
                break;
        }

        switch (arch)
        {
            case Architecture::x86:
                if (bits == BitMode::Bits64)
                    header.cpuType = CpuType::X86_64;
                else
                    header.cpuType = CpuType::X86;
                break;
            case Architecture::ARM:
                if (bits == BitMode::Bits64)
                    header.cpuType = CpuType::Arm64;
                else
                    header.cpuType = CpuType::Arm;
                break;
            case Architecture::RISC_V:
                if (bits == BitMode::Bits64)
                    header.cpuType = CpuType::RiscV64;
                else
                    header.cpuType = CpuType::RiscV32;
                break;
            default:
                header.cpuType = CpuType::None;
                break;
        }

        header.cpuSubtype = 0;
        header.filetype = FileType::Object;
        header.flags = HFlags::SubsectionsViaSymbols;

        return header;
    }

    Data create(BitMode bits, Architecture arch, Endianness endianness, Encoded encoded, Parsed parsed)
    {
        Data data;
        data.header = createHeader(bits, arch, endianness);
        
        data.header.commandCount;   //TODO
        data.header.commandSize;    //TODO

        return data;
    }

    uint64_t writeHeader(std::ofstream& out, Endianness endianness, const Header& header)
    {
        Endian::write(out, header.magic, 4, endianness);
        
        Endian::write(out, static_cast<uint32_t>(header.cpuType), endianness);
        Endian::write(out, header.cpuSubtype, endianness);
        Endian::write(out, static_cast<uint32_t>(header.filetype), endianness);
        Endian::write(out, header.commandCount, endianness);
        Endian::write(out, header.commandSize, endianness);
        Endian::write(out, header.flags, endianness);

        bool is64Bit = (std::memcmp(header.magic, Magic::Magic64, 4) == 0 || 
                    std::memcmp(header.magic, Magic::Cigam64, 4) == 0);

        if (is64Bit)
        {
            Endian::write(out, header.reserved, endianness);
            return sizeof(Header);
        }
        else
            return sizeof(Header) - 4;
    }

    void write(std::ofstream& out, Endianness endianness, Data& data)
    {
        uint64_t offset = writeHeader(out, endianness, data.header);
    }
}