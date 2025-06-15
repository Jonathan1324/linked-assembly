#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

namespace x86 {
    namespace bits32 {
        const std::unordered_map<std::string, uint32_t> registers = 
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
    }
}