#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <cstdint>

namespace x86 {
    namespace bits8 {
        static const std::unordered_map<std::string, uint32_t> registers = 
        {
            // Lower byte registers
            {"al", 0x0},
            {"cl", 0x1},
            {"dl", 0x2},
            {"bl", 0x3},

            // Higher byte registers
            {"ah", 0x4},
            {"ch", 0x5},
            {"dh", 0x6},
            {"bh", 0x7},
        };

        static const std::unordered_map<std::string, uint32_t> registers64 = 
        {
            // Extended low byte registers (r8b - r15b)
            {"r8b", 0x8},
            {"r9b", 0x9},
            {"r10b", 0xA},
            {"r11b", 0xB},
            {"r12b", 0xC},
            {"r13b", 0xD},
            {"r14b", 0xE},
            {"r15b", 0xF},
        };
    }

    namespace bits16 {
        static const std::unordered_map<std::string, uint32_t> registers = 
        {
            {"ax", 0x0},
            {"cx", 0x1},
            {"dx", 0x2},
            {"bx", 0x3},
            {"sp", 0x4},
            {"bp", 0x5},
            {"si", 0x6},
            {"di", 0x7},
        };

        static const std::unordered_map<std::string, uint32_t> segmentRegisters = 
        {
            {"es", 0x0},
            {"cs", 0x1},
            {"ss", 0x2},
            {"ds", 0x3},
            {"fs", 0x4},
            {"gs", 0x5},
        };

        constexpr const std::string_view flagsRegister = "flags";


        static const std::unordered_map<std::string, uint32_t> registers64 = 
        {
            // Extended registers 16-bit (r8w - r15w)
            {"r8w", 0x8},
            {"r9w", 0x9},
            {"r10w", 0xA},
            {"r11w", 0xB},
            {"r12w", 0xC},
            {"r13w", 0xD},
            {"r14w", 0xE},
            {"r15w", 0xF},
        };
    }

    namespace bits32 {
        constexpr const uint8_t prefix16 = 0x66;
        
        static const std::unordered_map<std::string, uint32_t> registers = 
        {
            {"eax", 0x0},
            {"ecx", 0x1},
            {"edx", 0x2},
            {"ebx", 0x3},
            {"esp", 0x4},
            {"ebp", 0x5},
            {"esi", 0x6},
            {"edi", 0x7},
        };

        static const std::unordered_map<std::string, uint32_t> controlRegisters = 
        {
            {"cr0", 0x0},
            //cr1: reserved
            {"cr2", 0x2},
            {"cr3", 0x3},
            {"cr4", 0x4},
        };

        static const std::unordered_map<std::string, uint32_t> debugRegisters = 
        {
            {"dr0", 0x0},
            {"dr1", 0x1},
            {"dr2", 0x2},
            {"dr3", 0x3},
            {"dr6", 0x6},
            {"dr7", 0x7},
        };

        constexpr const std::string_view flagsRegister = "eflags";

        static const std::unordered_map<std::string, uint32_t> mmxRegisters = 
        {
            {"mm0", 0x0},
            {"mm1", 0x1},
            {"mm2", 0x2},
            {"mm3", 0x3},
            {"mm4", 0x4},
            {"mm5", 0x5},
            {"mm6", 0x6},
            {"mm7", 0x7},
        };

        static const std::unordered_map<std::string, uint32_t> xmmRegisters = 
        {
            {"xmm0", 0x0}, 
            {"xmm1", 0x1}, 
            {"xmm2", 0x2}, 
            {"xmm3", 0x3},
            {"xmm4", 0x4}, 
            {"xmm5", 0x5}, 
            {"xmm6", 0x6}, 
            {"xmm7", 0x7},
        };

        static const std::unordered_map<std::string, uint32_t> fpuRegisters =
        {
            {"st0", 0x0},
            {"st1", 0x1},
            {"st2", 0x2},
            {"st3", 0x3},
            {"st4", 0x4},
            {"st5", 0x5},
            {"st6", 0x6},
            {"st7", 0x7},
        };


        static const std::unordered_map<std::string, uint32_t> registers64 = 
        {
            // Extended registers 32-bit (r8d - r15d)
            {"r8d", 0x8},
            {"r9d", 0x9},
            {"r10d", 0xA},
            {"r11d", 0xB},
            {"r12d", 0xC},
            {"r13d", 0xD},
            {"r14d", 0xE},
            {"r15d", 0xF},
        };
    }

    namespace bits64 {
        static const std::unordered_map<std::string, uint32_t> registers = 
        {
            {"rax", 0x0},
            {"rcx", 0x1},
            {"rdx", 0x2},
            {"rbx", 0x3},
            {"rsp", 0x4},
            {"rbp", 0x5},
            {"rsi", 0x6},
            {"rdi", 0x7},

            // Extended registers
            {"r8",  0x8},
            {"r9",  0x9},
            {"r10", 0xA},
            {"r11", 0xB},
            {"r12", 0xC},
            {"r13", 0xD},
            {"r14", 0xE},
            {"r15", 0xF},
        };

        static const std::unordered_map<std::string, uint32_t> controlRegisters = 
        {
            {"cr0", 0x0},
            //cr1: reserved
            {"cr2", 0x2},
            {"cr3", 0x3},
            {"cr4", 0x4},
            {"cr8", 0x8},  // Only valid in 64-bit mode
        };

        static const std::unordered_map<std::string, uint32_t> debugRegisters = 
        {
            {"dr0", 0x0},
            {"dr1", 0x1},
            {"dr2", 0x2},
            {"dr3", 0x3},
            {"dr6", 0x6},
            {"dr7", 0x7},
        };

        static const std::unordered_map<std::string, uint32_t> xmmRegisters =
        {
            {"xmm0", 0x0},
            {"xmm1", 0x1},
            {"xmm2", 0x2},
            {"xmm3", 0x3},
            {"xmm4", 0x4},
            {"xmm5", 0x5},
            {"xmm6", 0x6},
            {"xmm7", 0x7},
            {"xmm8", 0x8},
            {"xmm9", 0x9},
            {"xmm10", 0xA},
            {"xmm11", 0xB},
            {"xmm12", 0xC},
            {"xmm13", 0xD},
            {"xmm14", 0xE},
            {"xmm15", 0xF},
        };

        constexpr const std::string_view flagsRegister = "rflags";
    }
}