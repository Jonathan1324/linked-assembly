#pragma once

#include "binary.hpp"

namespace Binary {
    uint64_t getAlignment64(std::string name);
    uint64_t getAlignment32(std::string name);
}