#pragma once

#include <unordered_set>
#include <string_view>

namespace x86
{
    static const std::unordered_set<std::string_view> registers = {
        // 8-bit
        "al", "cl", "dl", "bl", // lower byte
        "ah", "ch", "dh", "bh", // higher byte

        "r8b", "r9b", "r10b", "r11b",
        "r12b", "r13b", "r14b", "r15b",

        // 16-bit
        "ax", "cx", "dx", "bx",
        "sp", "bp", "si", "di",
        "es", "cs", "ss", "ds", "fs", "gs",

        "flags",

        "r8w", "r9w", "r10w", "r11w",
        "r12w", "r13w", "r14w", "r15w",

        // 32-bit
        "eax", "ecx", "edx", "ebx",
        "esp", "ebp", "esi", "edi",

        "cr0", /*"cr1",*/ "cr2", "cr3", "cr4",

        "dr0", "dr1", "dr2", "dr3", /*"dr4", "dr5",*/ "dr6", "dr7",

        "eflags",

        "mm0", "mm1", "mm2", "mm3", "mm4", "mm5", "mm6", "mm7",
        "xmm0", "xmm1", "xmm2", "xmm3", "xmm4", "xmm5", "xmm6", "xmm7",

        "st0", "st1", "st2", "st3", "st4", "st5", "st6", "st7",

        "r8d", "r9d", "r10d", "r11d",
        "r12d", "r13d", "r14d", "r15d",

        // 64-bit
        "rax", "rcx", "rdx", "rbx",
        "rsp", "rbp", "rsi", "rdi",
        "r8", "r9", "r10", "r11",
        "r12", "r13", "r14", "r15",

        "cr8",

        "xmm8", "xmm9", "xmm10", "xmm11", "xmm12", "xmm13", "xmm14", "xmm15",

        "rflags"
    };
}