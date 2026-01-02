#pragma once

#include <unordered_map>
#include <string_view>
#include <cstdint>

namespace x86
{
    enum Registers : uint64_t
    {
        AL, BL, CL, DL,
        AH, BH, CH, DH,
        SPL, BPL, SIL, DIL,
        R8B, R9B, R10B, R11B,
        R12B, R13B, R14B, R15B,

        AX, BX, CX, DX,
        SP, BP, SI, DI,
        ES, CS, SS, DS, FS, GS,
        FLAGS,
        R8W, R9W, R10W, R11W,
        R12W, R13W, R14W, R15W,

        IP,

        EAX, EBX, ECX, EDX,
        ESP, EBP, ESI, EDI,
        R8D, R9D, R10D, R11D,
        R12D, R13D, R14D, R15D,
        CR0, /*CR1,*/ CR2, CR3, CR4, CR5, CR6, CR7,
        DR0, DR1, DR2, DR3, /*DR4, DR5,*/ DR6, DR7,
        TR0, TR1, TR2, TR3, TR4, TR5, TR6, TR7,
        ST0, ST1, ST2, ST3, ST4, ST5, ST6, ST7,
        EFLAGS,

        MM0, MM1, MM2, MM3, MM4, MM5, MM6, MM7,

        EIP,

        RAX, RBX, RCX, RDX,
        RSP, RBP, RSI, RDI,
        R8, R9, R10, R11,
        R12, R13, R14, R15,
        CR8, CR9, CR10, CR11, CR12, CR13, CR14, CR15,
        DR8, DR9, DR10, DR11, DR12, DR13, DR14, DR15,
        RFLAGS,

        RIP,

        XMM0, XMM1, XMM2, XMM3, XMM4, XMM5, XMM6, XMM7,
        XMM8, XMM9, XMM10, XMM11, XMM12, XMM13, XMM14, XMM15,

        YMM0, YMM1, YMM2, YMM3, YMM4, YMM5, YMM6, YMM7,
        YMM8, YMM9, YMM10, YMM11, YMM12, YMM13, YMM14, YMM15,

        ZMM0, ZMM1, ZMM2, ZMM3, ZMM4, ZMM5, ZMM6, ZMM7,
        ZMM8, ZMM9, ZMM10, ZMM11, ZMM12, ZMM13, ZMM14, ZMM15,
        ZMM16, ZMM17, ZMM18, ZMM19, ZMM20, ZMM21, ZMM22, ZMM23,
        ZMM24, ZMM25, ZMM26, ZMM27, ZMM28, ZMM29, ZMM30, ZMM31,

        K0, K1, K2, K3, K4, K5, K6, K7,

        MXCSR,

        BND0, BND1, BND2, BND3,
        BNDCFGU, BNDCFGS,

        XCR0, PKRU
    };

    static const std::unordered_map<std::string_view, Registers> registers = {
        {"al", AL}, {"bl", BL}, {"cl", CL}, {"dl", DL},
        {"ah", AH}, {"bh", BH}, {"ch", CH}, {"dh", DH},
        {"spl", SPL}, {"bpl", BPL}, {"sil", SIL}, {"dil", DIL},
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
        {"r8d", R8D}, {"r9d", R9D}, {"r10d", R10D}, {"r11d", R11D},
        {"r12d", R12D}, {"r13d", R13D}, {"r14d", R14D}, {"r15d", R15D},

        // Control registers
        {"cr0", CR0}, /*{"cr1": CR1,}*/  // CR1 is not used in x86-64
        {"cr2", CR2}, 
        {"cr3", CR3},
        {"cr4", CR4},
        {"cr5", CR5},
        {"cr6", CR6},
        {"cr7", CR7},

        // Debug registers
        {"dr0", DR0},
        {"dr1", DR1},
        {"dr2", DR2},
        {"dr3", DR3},
        /*{"dr4": DR4, "dr5": DR5,}*/  // DR4 and DR5 are not used in x86-64
        {"dr6", DR6},
        {"dr7", DR7},
        {"dr8", DR8},
        {"dr9", DR9},
        {"dr10", DR10},
        {"dr11", DR11},
        {"dr12", DR12},
        {"dr13", DR13},
        {"dr14", DR14},
        {"dr15", DR15},

        // Flags register
        { "eflags" , EFLAGS },

        // 64-bit registers
        {"rax", RAX}, {"rbx", RBX}, {"rcx", RCX}, {"rdx", RDX},
        {"rsp", RSP}, {"rbp", RBP}, {"rsi", RSI}, {"rdi", RDI},
        {"r8", R8}, {"r9", R9}, {"r10", R10}, {"r11", R11},
        {"r12", R12}, {"r13", R13}, {"r14", R14}, {"r15", R15},

        {"cr8", CR8}, {"cr9", CR9}, {"cr10", CR10}, {"cr11", CR11},
        {"cr12", CR12}, {"cr13", CR13}, {"cr14", CR14}, {"cr15", CR15},

        {"rflags", RFLAGS},

        // Task registers
        {"tr0", TR0}, {"tr1", TR1}, {"tr2", TR2}, {"tr3", TR3},
        {"tr4", TR4}, {"tr5", TR5}, {"tr6", TR6}, {"tr7", TR7},

        // FPU registers
        {"st0", ST0}, {"st1", ST1}, {"st2", ST2}, {"st3", ST3},
        {"st4", ST4}, {"st5", ST5}, {"st6", ST6}, {"st7", ST7},

        // MMX registers
        {"mm0", MM0}, {"mm1", MM1}, {"mm2", MM2}, {"mm3", MM3},
        {"mm4", MM4}, {"mm5", MM5}, {"mm6", MM6}, {"mm7", MM7},

        // XMM registers
        {"xmm0", XMM0}, {"xmm1", XMM1}, {"xmm2", XMM2}, {"xmm3", XMM3},
        {"xmm4", XMM4}, {"xmm5", XMM5}, {"xmm6", XMM6}, {"xmm7", XMM7},
        {"xmm8", XMM8}, {"xmm9", XMM9}, {"xmm10", XMM10}, {"xmm11", XMM11},
        {"xmm12", XMM12}, {"xmm13", XMM13}, {"xmm14", XMM14}, {"xmm15", XMM15},

        // YMM registers
        {"ymm0", YMM0}, {"ymm1", YMM1}, {"ymm2", YMM2}, {"ymm3", YMM3},
        {"ymm4", YMM4}, {"ymm5", YMM5}, {"ymm6", YMM6}, {"ymm7", YMM7},
        {"ymm8", YMM8}, {"ymm9", YMM9}, {"ymm10", YMM10}, {"ymm11", YMM11},
        {"ymm12", YMM12}, {"ymm13", YMM13}, {"ymm14", YMM14}, {"ymm15", YMM15},

        // ZMM registers
        {"zmm0", ZMM0}, {"zmm1", ZMM1}, {"zmm2", ZMM2}, {"zmm3", ZMM3},
        {"zmm4", ZMM4}, {"zmm5", ZMM5}, {"zmm6", ZMM6}, {"zmm7", ZMM7},
        {"zmm8", ZMM8}, {"zmm9", ZMM9}, {"zmm10", ZMM10}, {"zmm11", ZMM11},
        {"zmm12", ZMM12}, {"zmm13", ZMM13}, {"zmm14", ZMM14}, {"zmm15", ZMM15},
        {"zmm16", ZMM16}, {"zmm17", ZMM17}, {"zmm18", ZMM18}, {"zmm19", ZMM19},
        {"zmm20", ZMM20}, {"zmm21", ZMM21}, {"zmm22", ZMM22}, {"zmm23", ZMM23},
        {"zmm24", ZMM24}, {"zmm25", ZMM25}, {"zmm26", ZMM26}, {"zmm27", ZMM27},
        {"zmm28", ZMM28}, {"zmm29", ZMM29}, {"zmm30", ZMM30}, {"zmm31", ZMM31},

        // OpMask
        {"k0", K0}, {"k1", K1}, {"k2", K2}, {"k3", K3},
        {"k4", K4}, {"k5", K5}, {"k6", K6}, {"k7", K7},

        // SSE control and status
        {"mxcsr", MXCSR},

        // Bound registers
        {"bnd0", BND0}, {"bnd1", BND1}, {"bnd2", BND2}, {"bnd3", BND3},
        {"bndcfgu", BNDCFGU}, {"bndcfgs", BNDCFGS},

        // other
        {"xcr0", XCR0}, {"pkru", PKRU}
    };
}