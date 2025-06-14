#pragma once

#include <unordered_map>
#include <string>
#include <cstdint>

namespace x86 {
    namespace bits32 {
        std::unordered_map<std::string, uint8_t> reg_map = {
            {"eax", 0},
            {"ecx", 1},
            {"edx", 2},
            {"ebx", 3},
            {"esp", 4},
            {"ebp", 5},
            {"esi", 6},
            {"edi", 7}
        };
    }
}