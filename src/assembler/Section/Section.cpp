#include "Section.hpp"

void Section::setAlignments(std::vector<Encoder::Section>& sections, Architecture arch, BitMode bits)
{
    for (auto& section : sections)
    {
        if (section.align != 0)
            continue;
        
        switch(arch)
        {
            case Architecture::x86:
                switch(bits)
                {
                    case BitMode::Bits16: section.align = 2; break;
                    case BitMode::Bits32: section.align = 4; break;
                    case BitMode::Bits64: section.align = 8; break;
                }
                break;

            case Architecture::ARM:
            case Architecture::RISC_V:
            default:
                break;
        }
    }
}