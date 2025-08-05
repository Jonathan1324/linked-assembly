#pragma once

#include <cstdint>

namespace x86
{
    enum Instructions : uint64_t
    {
        NOP,
        INT,
        MOV
    };
}