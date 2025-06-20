#pragma once

#include <cinttypes>
#include <string>

#include <Architecture.hpp>
#include "../../encoder/encoder.hpp"

namespace MACHO {
    namespace Magic {
        constexpr unsigned char Magic32[4] = { 0xfe, 0xed, 0xfa, 0xce };
        constexpr unsigned char Magic64[4] = { 0xfe, 0xed, 0xfa, 0xcf };
        constexpr unsigned char Cigam32[4] = { 0xce, 0xfa, 0xed, 0xfe };
        constexpr unsigned char Cigam64[4] = { 0xcf, 0xfa, 0xed, 0xfe };
    }

    enum class CpuType : uint32_t {
        None      = 0,
        X86       = 7,
        X86_64    = (7 | 0x01000000),
        Arm       = 12,
        Arm64     = (12 | 0x01000000), 
        RiscV32   = 0xF3,
        RiscV64   = (0xF3 | 0x01000000)
    };

    enum class FileType : uint32_t {
        None           = 0,
        Object         = 1,    // Relocatable object file
        Execute        = 2,    // Executable file
        FixedVMExecute = 3,    // Fixed VM executable file
        Core           = 4,    // Core dump file
        PreloadedExe   = 5,    // Preloaded executable file
        DynamicallyLinkedSharedLib = 6, // Dynamically linked shared library
        Bundle         = 7,    // Dynamically linked bundle file
        Dylinker       = 8,    // Dynamic link editor
        LazyBundle     = 9,    // Lazy dynamically linked bundle file
        KernelModule   = 10    // Kernel module file
    };

    namespace HFlags {
        constexpr uint32_t NoUndefs           = 0x1;
        constexpr uint32_t IncrementalLink    = 0x2;
        constexpr uint32_t DyldLink           = 0x4;
        constexpr uint32_t BindAtLoad         = 0x8;
        constexpr uint32_t Prebound           = 0x10;
        constexpr uint32_t SplitSegs          = 0x20;
        constexpr uint32_t LazyInit           = 0x40;
        constexpr uint32_t TwoLevel           = 0x80;
        constexpr uint32_t ForceFlat          = 0x100;
        constexpr uint32_t NoMultiDefs        = 0x200;
        constexpr uint32_t NoFixPrebinding    = 0x400;
        constexpr uint32_t Prebindable        = 0x800;
        constexpr uint32_t AllModsBound       = 0x1000;
        constexpr uint32_t SubsectionsViaSymbols = 0x2000;
        constexpr uint32_t Canonical          = 0x4000;
        constexpr uint32_t WeakDefines        = 0x8000;
        constexpr uint32_t BindOverridden     = 0x10000;
        constexpr uint32_t AllowStackExecution = 0x20000;
        constexpr uint32_t RootSafe           = 0x40000;
        constexpr uint32_t SetuidSafe         = 0x80000;
        constexpr uint32_t NoReexportedDylibs = 0x100000;
        constexpr uint32_t PIE                = 0x200000;
        constexpr uint32_t DeadStrippableDylib = 0x400000;
        constexpr uint32_t HasTLVDescriptors  = 0x800000;
        constexpr uint32_t NoHeapExecution    = 0x1000000;
        constexpr uint32_t AppExtensionSafe   = 0x2000000;
    }

    struct Header {
        unsigned char magic[4];
        CpuType cpuType;
        uint32_t cpuSubtype;
        FileType filetype;
        uint32_t commandCount;
        uint32_t commandSize;
        uint32_t flags;

        // 64 bit
        uint32_t reserved = 0;
    } __attribute__((packed));

    struct Data {
        Header header;
    };

    Data create(BitMode bits, Architecture arch, Endianness endianness, Encoded encoded, Parsed parsed, Context& context);
    void write(std::ofstream& out, Endianness endianness, Data& data, Context& context);
}