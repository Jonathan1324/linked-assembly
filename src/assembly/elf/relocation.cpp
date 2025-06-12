#include "relocation.hpp"
#include "../util/buffer.hpp"

namespace ELF {
    sectionBuffer encodeRelocations(std::vector<Relocation> relocations, Bitness bits)
    {
        sectionBuffer buffer;
        
        for (const auto& relocation : relocations)
        {
            uint64_t symbolIndex = 0;   //TODO
            uint64_t symbolType = 0;    //TODO

            if (bits == Bitness::Bits64)
            {
                Rela64 rela;
                rela.offset = relocation.offsetInSection;
                rela.info = ((symbolIndex << 32) | (symbolType & 0xFFFFFFFF));;
                rela.addend = relocation.addend;

                writeToBuffer(buffer, rela);
            }
            else
            {
                Rela32 rela;
                rela.offset = relocation.offsetInSection;
                rela.info = ((symbolIndex << 8) | (symbolType & 0xFF));;
                rela.addend = relocation.addend;

                writeToBuffer(buffer, rela);
            }
        }

        return buffer;
    }
}