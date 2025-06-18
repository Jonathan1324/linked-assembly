#pragma once

#include <optional>
#include <string>
#include <cstdbool>
#include <cstdint>

namespace x86 {
    namespace bits32 {
        struct MemoryOperand {
            // base reg
            std::optional<std::string> base;
            // index reg
            std::optional<std::string> index;

            // label
            std::optional<std::string> label;

            uint8_t scale = 1;
            int32_t displacement = 0;
        };

        enum class Memory {
            None,   // none
            Default,// default

            Byte,   // 8-bit
            Word,   // 16-bit
            Dword,  // 32-bit
            Fword,  // 48-bit (far pointers)
            Qword,  // 64-bit
            MMword, // 64-bit (MMX registers)
            Tbyte,  // 80-bit (x87 FPU)
            Xword,  // 80-bit alias
            Oword,  // 128-bit (SSE)
            XMMword,// 128-bit (XMM registers)
            Yword,  // 256-bit (AVX)
            Zword   // 512-bit (AVX-512)
        };

        MemoryOperand parseMem(const std::string& input);
        Memory isMemoryOperand(const std::string& op);
    }
}