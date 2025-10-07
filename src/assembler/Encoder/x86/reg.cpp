#include "Encoder.hpp"

#include <x86/Registers.hpp>

std::tuple<uint8_t, bool, bool> Encoder::x86::Encoder::getReg(uint64_t reg)
{
    switch (reg)
    {
        case ::x86::AL:     return std::make_tuple(0, false, false);
        case ::x86::CL:     return std::make_tuple(1, false, false);
        case ::x86::DL:     return std::make_tuple(2, false, false);
        case ::x86::BL:     return std::make_tuple(3, false, false);
        case ::x86::AH:     return std::make_tuple(4, false, false);
        case ::x86::CH:     return std::make_tuple(5, false, false);
        case ::x86::DH:     return std::make_tuple(6, false, false);
        case ::x86::BH:     return std::make_tuple(7, false, false);
        case ::x86::SPL:    return std::make_tuple(4, true,  false);
        case ::x86::BPL:    return std::make_tuple(5, true,  false);
        case ::x86::SIL:    return std::make_tuple(6, true,  false);
        case ::x86::DIL:    return std::make_tuple(7, true,  false);
        case ::x86::R8B:    return std::make_tuple(0, true,  true);
        case ::x86::R9B:    return std::make_tuple(1, true,  true);
        case ::x86::R10B:   return std::make_tuple(2, true,  true);
        case ::x86::R11B:   return std::make_tuple(3, true,  true);
        case ::x86::R12B:   return std::make_tuple(4, true,  true);
        case ::x86::R13B:   return std::make_tuple(5, true,  true);
        case ::x86::R14B:   return std::make_tuple(6, true,  true);
        case ::x86::R15B:   return std::make_tuple(7, true,  true);

        case ::x86::AX:     return std::make_tuple(0, false, false);
        case ::x86::CX:     return std::make_tuple(1, false, false);
        case ::x86::DX:     return std::make_tuple(2, false, false);
        case ::x86::BX:     return std::make_tuple(3, false, false);
        case ::x86::SP:     return std::make_tuple(4, false, false);
        case ::x86::BP:     return std::make_tuple(5, false, false);
        case ::x86::SI:     return std::make_tuple(6, false, false);
        case ::x86::DI:     return std::make_tuple(7, false, false);
        case ::x86::R8W:    return std::make_tuple(0, true,  true);
        case ::x86::R9W:    return std::make_tuple(1, true,  true);
        case ::x86::R10W:   return std::make_tuple(2, true,  true);
        case ::x86::R11W:   return std::make_tuple(3, true,  true);
        case ::x86::R12W:   return std::make_tuple(4, true,  true);
        case ::x86::R13W:   return std::make_tuple(5, true,  true);
        case ::x86::R14W:   return std::make_tuple(6, true,  true);
        case ::x86::R15W:   return std::make_tuple(7, true,  true);

        case ::x86::ES:     return std::make_tuple(0, false,  false);
        case ::x86::CS:     return std::make_tuple(1, false,  false);
        case ::x86::SS:     return std::make_tuple(2, false,  false);
        case ::x86::DS:     return std::make_tuple(3, false,  false);
        case ::x86::FS:     return std::make_tuple(4, false,  false);
        case ::x86::GS:     return std::make_tuple(5, false,  false);

        case ::x86::EAX:    return std::make_tuple(0, false, false);
        case ::x86::ECX:    return std::make_tuple(1, false, false);
        case ::x86::EDX:    return std::make_tuple(2, false, false);
        case ::x86::EBX:    return std::make_tuple(3, false, false);
        case ::x86::ESP:    return std::make_tuple(4, false, false);
        case ::x86::EBP:    return std::make_tuple(5, false, false);
        case ::x86::ESI:    return std::make_tuple(6, false, false);
        case ::x86::EDI:    return std::make_tuple(7, false, false);
        case ::x86::R8D:    return std::make_tuple(0, true,  true);
        case ::x86::R9D:    return std::make_tuple(1, true,  true);
        case ::x86::R10D:   return std::make_tuple(2, true,  true);
        case ::x86::R11D:   return std::make_tuple(3, true,  true);
        case ::x86::R12D:   return std::make_tuple(4, true,  true);
        case ::x86::R13D:   return std::make_tuple(5, true,  true);
        case ::x86::R14D:   return std::make_tuple(6, true,  true);
        case ::x86::R15D:   return std::make_tuple(7, true,  true);

        case ::x86::CR0:    return std::make_tuple(0, false, false);
        case ::x86::CR2:    return std::make_tuple(2, false, false);
        case ::x86::CR3:    return std::make_tuple(3, false, false);
        case ::x86::CR4:    return std::make_tuple(4, false, false);
        case ::x86::CR5:    return std::make_tuple(5, false, false);
        case ::x86::CR6:    return std::make_tuple(6, false, false);
        case ::x86::CR7:    return std::make_tuple(7, false, false);

        case ::x86::DR0:    return std::make_tuple(0, false, false);
        case ::x86::DR1:    return std::make_tuple(1, false, false);
        case ::x86::DR2:    return std::make_tuple(2, false, false);
        case ::x86::DR3:    return std::make_tuple(3, false, false);
        case ::x86::DR6:    return std::make_tuple(6, false, false);
        case ::x86::DR7:    return std::make_tuple(7, false, false);

        case ::x86::TR0:    return std::make_tuple(0, false, false);
        case ::x86::TR1:    return std::make_tuple(1, false, false);
        case ::x86::TR2:    return std::make_tuple(2, false, false);
        case ::x86::TR3:    return std::make_tuple(3, false, false);
        case ::x86::TR4:    return std::make_tuple(4, false, false);
        case ::x86::TR5:    return std::make_tuple(5, false, false);
        case ::x86::TR6:    return std::make_tuple(6, false, false);
        case ::x86::TR7:    return std::make_tuple(7, false, false);
    }
    throw Exception::InternalError("Unknown register");
}

uint8_t Encoder::x86::Encoder::getRegSize(uint64_t reg, BitMode mode)
{
    switch (reg)
    {
        case ::x86::AL: case ::x86::CL:
        case ::x86::DL: case ::x86::BL:
        case ::x86::AH: case ::x86::CH:
        case ::x86::DH: case ::x86::BH:
        case ::x86::SPL: case ::x86::BPL:
        case ::x86::SIL: case ::x86::DIL:
        case ::x86::R8B: case ::x86::R9B:
        case ::x86::R10B: case ::x86::R11B:
        case ::x86::R12B: case ::x86::R13B:
        case ::x86::R14B: case ::x86::R15B:
            return 8;

        case ::x86::AX: case ::x86::CX:
        case ::x86::DX: case ::x86::BX:
        case ::x86::SP: case ::x86::BP:
        case ::x86::SI: case ::x86::DI:
        case ::x86::R8W: case ::x86::R9W:
        case ::x86::R10W: case ::x86::R11W:
        case ::x86::R12W: case ::x86::R13W:
        case ::x86::R14W: case ::x86::R15W:
            return 16;

        case ::x86::ES: case ::x86::CS:
        case ::x86::SS: case ::x86::DS:
        case ::x86::FS: case ::x86::GS:
            return 16;

        case ::x86::EAX: case ::x86::EBX:
        case ::x86::ECX: case ::x86::EDX:
        case ::x86::ESP: case ::x86::EBP:
        case ::x86::ESI: case ::x86::EDI:
        case ::x86::R8D: case ::x86::R9D:
        case ::x86::R10D: case ::x86::R11D:
        case ::x86::R12D: case ::x86::R13D:
        case ::x86::R14D: case ::x86::R15D:
            return 32;


        case ::x86::CR0: case ::x86::CR2:
        case ::x86::CR3: case ::x86::CR4:
        case ::x86::CR5: case ::x86::CR6:
        case ::x86::CR7:
        case ::x86::DR0: case ::x86::DR1:
        case ::x86::DR2: case ::x86::DR3:
        case ::x86::DR6: case ::x86::DR7:
        case ::x86::TR0: case ::x86::TR1:
        case ::x86::TR2: case ::x86::TR3:
        case ::x86::TR4: case ::x86::TR5:
        case ::x86::TR6: case ::x86::TR7:
            if (mode == BitMode::Bits64) return 64;
            else return 32;
    }
    return 0;
}