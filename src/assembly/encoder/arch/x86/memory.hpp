#pragma once

#include <optional>
#include <string>
#include <cstdbool>
#include <cstdint>
#include "../../../util/string.hpp"

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

        MemoryOperand parseMem(const std::string& input);
        bool isMemoryOperand(const std::string& op);
    }
}