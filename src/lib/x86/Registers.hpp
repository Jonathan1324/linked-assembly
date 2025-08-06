#pragma once

#include <unordered_set>
#include <string_view>

namespace x86
{
    enum Registers : uint64_t
    {
        AL, BL, CL, DL,
        AH, BH, CH, DH,
        R8B, R9B, R10B, R11B,
        R12B, R13B, R14B, R15B,

        AX, BX, CX, DX,
        SP, BP, SI, DI,
        ES, CS, SS, DS, FS, GS,
        FLAGS,
        R8W, R9W, R10W, R11W,
        R12W, R13W, R14W, R15W,

        EAX, EBX, ECX, EDX,
        ESP, EBP, ESI, EDI,
        CR0, /*CR1,*/ CR2, CR3, CR4,
        DR0, DR1, DR2, DR3, /*DR4, DR5,*/ DR6, DR7,
        EFLAGS,
        MM0, MM1, MM2, MM3, MM4, MM5, MM6, MM7,
        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7,
        ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7,
        R8D, R9D, R10D, R11D,
        R12D, R13D, R14D, R15D,

        RAX, RBX, RCX, RDX,
        RSP, RBP, RSI, RDI,
        R8, R9, R10, R11,
        R12, R13, R14, R15,
        CR8,
        XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,
        RFLAGS
    };

    static const std::unordered_map<std::string_view, Registers> registers = {
        {"al", AL}, {"bl", BL}, {"cl", CL}, {"dl", DL},
        {"ah", AH}, {"bh", BH}, {"ch", CH}, {"dh", DH},
        {"r8b", R8B}, {"r9b", R9B}, {"r10b", R10B}, {"r11b", R11B},
        {"r12b", R12B}, {"r13b", R13B}, {"r14b", R14B}, {"r15b", R15B},

        {"ax", AX}, {"bx", BX}, {"cx", CX}, {"dx", DX},
        {"sp", SP}, {"bp", BP}, {"si", SI}, {"di", DI},
        {"es", ES}, {"cs", CS}, {"ss", SS}, {"ds", DS}, {"fs", FS}, {"gs", GS},
        {"flags", FLAGS},
        {"r8w", R8W}, {"r9w", R9W}, {"r10w", R10W}, {"r11w", R11W},
        {"r12w", R12W}, {"r13w", R13W}, {"r14w", R14W}, {"r15w", R15W},

        // 32-bit registers
        {"eax", EAX}, {"ebx", EBX}, {"ecx", ECX}, {"edx", EDX},
        {"esp", ESP}, {"ebp", EBP}, {"esi", ESI}, {"edi", EDI},

        // Control registers
        {"cr0", CR0}, /*{"cr1": CR1,}*/  // CR1 is not used in x86-64
        {"cr2", CR2}, 
        {"cr3", CR3},
        {"cr4", CR4},

        // Debug registers
        {"dr0", DR0},
        {"dr1", DR1},
        {"dr2", DR2},
        {"dr3", DR3},
        /*{"dr4": DR4, "dr5": DR5,}*/  // DR4 and DR5 are not used in x86-64
        {"dr6", DR6},
        {"dr7", DR7},

        // Flags register
        { "eflags" , EFLAGS },

        // MMX registers
        {"mm0", MM0}, {"mm1", MM1}, {"mm2", MM2}, {"mm3", MM3},
        {"mm4", MM4}, {"mm5", MM5}, {"mm6", MM6}, {"mm7", MM7},

        // XMM registers
        {"xmm0", XMM0}, {"xmm1", XMM1}, {"xmm2", XMM2}, {"xmm3", XMM3},
        {"xmm4", XMM4}, {"xmm5", XMM5}, {"xmm6", XMM6}, {"xmm7", XMM7},

        // FPU registers
        {"st0", ST0}, {"st1", ST1}, {"st2", ST2}, {"st3", ST3},
        {"st4", ST4}, {"st5", ST5}, {"st6", ST6}, {"st7", ST7},

        {"r8d", R8D}, {"r9d", R9D}, {"r10d", R10D}, {"r11d", R11D},
        {"r12d", R12D}, {"r13d", R13D}, {"r14d", R14D}, {"r15d", R15D},

        // 64-bit registers
        {"rax", RAX}, {"rbx", RBX}, {"rcx", RCX}, {"rdx", RDX},
        {"rsp", RSP}, {"rbp", RBP}, {"rsi", RSI}, {"rdi", RDI},
        {"r8", R8}, {"r9", R9}, {"r10", R10}, {"r11", R11},
        {"r12", R12}, {"r13", R13}, {"r14", R14}, {"r15", R15},

        {"cr8", CR8},
        {"xmm8", XMM8}, {"xmm9", XMM9}, {"xmm10", XMM10}, {"xmm11", XMM11},
        {"xmm12", XMM12}, {"xmm13", XMM13}, {"xmm14", XMM14}, {"xmm15", XMM15},

        {"rflags", RFLAGS}
    };
}