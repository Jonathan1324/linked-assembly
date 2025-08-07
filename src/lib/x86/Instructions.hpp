#pragma once

#include <cstdint>

namespace x86
{
    enum Instructions : uint64_t
    {
        // CONTROL
        NOP,
        HLT,

        // INTERRUPT
        INT,

        // FLAGS
        CLC, STC, CMC,
        CLD, STD,
        CLI, STI,
        LAHF, SAHF,

        // STACK
        PUSHF, POPF,
        PUSHFD, POPFD,
        PUSHFQ, POPFQ,

    };
}