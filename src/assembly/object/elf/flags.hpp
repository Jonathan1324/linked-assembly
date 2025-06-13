#pragma once

#include <cinttypes>

namespace ELF {
    namespace ARM {
        namespace Flags32 {
            constexpr uint32_t relocationExec = 0x01;
            constexpr uint32_t hasEntry = 0x02;
            constexpr uint32_t Interwork = 0x04;
            constexpr uint32_t apcs26 = 0x08;
            constexpr uint32_t apcsFloat = 0x10;
            constexpr uint32_t pic = 0x20;
            constexpr uint32_t align8 = 0x40;
            constexpr uint32_t newABI = 0x80;
            constexpr uint32_t oldABI = 0x100;
            constexpr uint32_t softFloat = 0x200;
            constexpr uint32_t vfpFloat = 0x400;
            constexpr uint32_t maverickFloat = 0x800;
        }
    }

    namespace RISC_V {
        namespace Flags {
            constexpr uint32_t rvc = 0x1;
            constexpr uint32_t floatABI_soft = 0x0;
            constexpr uint32_t floatABI_single = 0x2;
            constexpr uint32_t floatABI_double = 0x4;
            constexpr uint32_t floatABI_quad = 0x8;
        }
    }
}