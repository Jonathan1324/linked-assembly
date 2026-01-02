#include "Encoder.hpp"

#include <x86/Registers.hpp>

std::tuple<uint8_t, bool, bool> x86::Encoder::getReg(uint64_t reg)
{
    switch (reg)
    {
        case AL:     return std::make_tuple(0, false, false);
        case CL:     return std::make_tuple(1, false, false);
        case DL:     return std::make_tuple(2, false, false);
        case BL:     return std::make_tuple(3, false, false);
        case AH:     return std::make_tuple(4, false, false);
        case CH:     return std::make_tuple(5, false, false);
        case DH:     return std::make_tuple(6, false, false);
        case BH:     return std::make_tuple(7, false, false);
        case SPL:    return std::make_tuple(4, true,  false);
        case BPL:    return std::make_tuple(5, true,  false);
        case SIL:    return std::make_tuple(6, true,  false);
        case DIL:    return std::make_tuple(7, true,  false);
        case R8B:    return std::make_tuple(0, true,  true);
        case R9B:    return std::make_tuple(1, true,  true);
        case R10B:   return std::make_tuple(2, true,  true);
        case R11B:   return std::make_tuple(3, true,  true);
        case R12B:   return std::make_tuple(4, true,  true);
        case R13B:   return std::make_tuple(5, true,  true);
        case R14B:   return std::make_tuple(6, true,  true);
        case R15B:   return std::make_tuple(7, true,  true);

        case AX:     return std::make_tuple(0, false, false);
        case CX:     return std::make_tuple(1, false, false);
        case DX:     return std::make_tuple(2, false, false);
        case BX:     return std::make_tuple(3, false, false);
        case SP:     return std::make_tuple(4, false, false);
        case BP:     return std::make_tuple(5, false, false);
        case SI:     return std::make_tuple(6, false, false);
        case DI:     return std::make_tuple(7, false, false);
        case R8W:    return std::make_tuple(0, true,  true);
        case R9W:    return std::make_tuple(1, true,  true);
        case R10W:   return std::make_tuple(2, true,  true);
        case R11W:   return std::make_tuple(3, true,  true);
        case R12W:   return std::make_tuple(4, true,  true);
        case R13W:   return std::make_tuple(5, true,  true);
        case R14W:   return std::make_tuple(6, true,  true);
        case R15W:   return std::make_tuple(7, true,  true);

        case ES:     return std::make_tuple(0, false,  false);
        case CS:     return std::make_tuple(1, false,  false);
        case SS:     return std::make_tuple(2, false,  false);
        case DS:     return std::make_tuple(3, false,  false);
        case FS:     return std::make_tuple(4, false,  false);
        case GS:     return std::make_tuple(5, false,  false);

        case EAX:    return std::make_tuple(0, false, false);
        case ECX:    return std::make_tuple(1, false, false);
        case EDX:    return std::make_tuple(2, false, false);
        case EBX:    return std::make_tuple(3, false, false);
        case ESP:    return std::make_tuple(4, false, false);
        case EBP:    return std::make_tuple(5, false, false);
        case ESI:    return std::make_tuple(6, false, false);
        case EDI:    return std::make_tuple(7, false, false);
        case R8D:    return std::make_tuple(0, true,  true);
        case R9D:    return std::make_tuple(1, true,  true);
        case R10D:   return std::make_tuple(2, true,  true);
        case R11D:   return std::make_tuple(3, true,  true);
        case R12D:   return std::make_tuple(4, true,  true);
        case R13D:   return std::make_tuple(5, true,  true);
        case R14D:   return std::make_tuple(6, true,  true);
        case R15D:   return std::make_tuple(7, true,  true);

        case RAX:    return std::make_tuple(0, false, false);
        case RCX:    return std::make_tuple(1, false, false);
        case RDX:    return std::make_tuple(2, false, false);
        case RBX:    return std::make_tuple(3, false, false);
        case RSP:    return std::make_tuple(4, false, false);
        case RBP:    return std::make_tuple(5, false, false);
        case RSI:    return std::make_tuple(6, false, false);
        case RDI:    return std::make_tuple(7, false, false);
        case R8:     return std::make_tuple(0, true,  true);
        case R9:     return std::make_tuple(1, true,  true);
        case R10:    return std::make_tuple(2, true,  true);
        case R11:    return std::make_tuple(3, true,  true);
        case R12:    return std::make_tuple(4, true,  true);
        case R13:    return std::make_tuple(5, true,  true);
        case R14:    return std::make_tuple(6, true,  true);
        case R15:    return std::make_tuple(7, true,  true);

        case CR0:    return std::make_tuple(0, false, false);
        case CR2:    return std::make_tuple(2, false, false);
        case CR3:    return std::make_tuple(3, false, false);
        case CR4:    return std::make_tuple(4, false, false);
        case CR5:    return std::make_tuple(5, false, false);
        case CR6:    return std::make_tuple(6, false, false);
        case CR7:    return std::make_tuple(7, false, false);
        case CR8:    return std::make_tuple(0, false, false);
        case CR9:    return std::make_tuple(1, false, false);
        case CR10:   return std::make_tuple(2, false, false);
        case CR11:   return std::make_tuple(3, false, false);
        case CR12:   return std::make_tuple(4, false, false);
        case CR13:   return std::make_tuple(5, false, false);
        case CR14:   return std::make_tuple(6, false, false);
        case CR15:   return std::make_tuple(7, false, false);

        case DR0:    return std::make_tuple(0, false, false);
        case DR1:    return std::make_tuple(1, false, false);
        case DR2:    return std::make_tuple(2, false, false);
        case DR3:    return std::make_tuple(3, false, false);
        case DR6:    return std::make_tuple(6, false, false);
        case DR7:    return std::make_tuple(7, false, false);
        case DR8:    return std::make_tuple(0, false, false);
        case DR9:    return std::make_tuple(1, false, false);
        case DR10:   return std::make_tuple(2, false, false);
        case DR11:   return std::make_tuple(3, false, false);
        case DR12:   return std::make_tuple(4, false, false);
        case DR13:   return std::make_tuple(5, false, false);
        case DR14:   return std::make_tuple(6, false, false);
        case DR15:   return std::make_tuple(7, false, false);

        case TR0:    return std::make_tuple(0, false, false);
        case TR1:    return std::make_tuple(1, false, false);
        case TR2:    return std::make_tuple(2, false, false);
        case TR3:    return std::make_tuple(3, false, false);
        case TR4:    return std::make_tuple(4, false, false);
        case TR5:    return std::make_tuple(5, false, false);
        case TR6:    return std::make_tuple(6, false, false);
        case TR7:    return std::make_tuple(7, false, false);
    }
    throw Exception::InternalError("Unknown register", -1, -1);
}

uint8_t x86::Encoder::getRegSize(uint64_t reg, BitMode mode)
{
    switch (reg)
    {
        case AL: case CL:
        case DL: case BL:
        case AH: case CH:
        case DH: case BH:
        case SPL: case BPL:
        case SIL: case DIL:
        case R8B: case R9B:
        case R10B: case R11B:
        case R12B: case R13B:
        case R14B: case R15B:
            return 8;

        case AX: case CX:
        case DX: case BX:
        case SP: case BP:
        case SI: case DI:
        case R8W: case R9W:
        case R10W: case R11W:
        case R12W: case R13W:
        case R14W: case R15W:
            return 16;

        case ES: case CS:
        case SS: case DS:
        case FS: case GS:
            return 16;

        case EAX: case EBX:
        case ECX: case EDX:
        case ESP: case EBP:
        case ESI: case EDI:
        case R8D: case R9D:
        case R10D: case R11D:
        case R12D: case R13D:
        case R14D: case R15D:
            return 32;

        case RAX: case RBX:
        case RCX: case RDX:
        case RSP: case RBP:
        case RSI: case RDI:
        case R8: case R9:
        case R10: case R11:
        case R12: case R13:
        case R14: case R15:
            return 64;


        case CR0: case CR2:
        case CR3: case CR4:
        case CR5: case CR6:
        case CR7: case CR8:
        case CR9: case CR10:
        case CR11: case CR12:
        case CR13: case CR14:
        case CR15:
        case DR0: case DR1:
        case DR2: case DR3:
        case DR6: case DR7:
        case DR8: case DR9:
        case DR10: case DR11:
        case DR12: case DR13:
        case DR14: case DR15:
        case TR0: case TR1:
        case TR2: case TR3:
        case TR4: case TR5:
        case TR6: case TR7:
            if (mode == BitMode::Bits64) return 64;
            else return 32;
    }
    return 0;
}