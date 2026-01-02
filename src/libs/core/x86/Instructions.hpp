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
        INT, IRET, IRETQ, IRETD,
        SYSCALL, SYSRET,
        SYSENTER, SYSEXIT,

        // FLAGS
        CLC, STC, CMC,
        CLD, STD,
        CLI, STI,
        LAHF, SAHF,

        // STACK
        PUSHA, POPA,
        PUSHAD, POPAD,
        PUSHF, POPF,
        PUSHFD, POPFD,
        PUSHFQ, POPFQ,

        // DATA
        MOV,

    };
}