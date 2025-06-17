#include "arguments.hpp"

#include "Exception.hpp"

#include "../version.h"
#include "util/string.hpp"

#include "cli/version.h"
#include "cli/help.h"

void parseArguments(int argc, const char *argv[],
                    std::string& input, std::string& output,
                    BitMode& bits, Architecture& arch, Format& format,
                    Endianness& endianness, bool& debug,
                    Context& context)
{
    bits = BitMode::Bits64;
    #ifdef __x86_64__
        // 64 bit - bitMode = BitMode::Bits64;
    #elif defined(__i386__)
        bits = BitMode::Bits32;
    #elif defined(__arm__)
        bits = BitMode::Bits32;
    #elif defined(__aarch64__)
        // 64 bit - bitMode = BitMode::Bits64;
    #endif

    arch = Architecture::x86;
    #if defined(__aarch64__) || defined(__arm__) || defined(_M_ARM)
        arch = Architecture::ARM;
    #elif defined(__riscv)
        arch = Architecture::RISC_V;
    #endif

    format = Format::ELF;
    #if defined(_WIN32) || defined(_WIN64)
        format = Format::COFF;
    #elif defined(__APPLE__) && defined(__MACH__)
        format = Format::MACHO;
    #elif defined(__linux__)
        // ELF - format = Format::ELF;
    #elif defined(__unix__)
        // ELF - format = Format::ELF;
    #endif

    endianness = Endianness::Little;

    if (argc < 2)
    {
        throw Exception::ArgumentError("No input file specified");
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
    {
        printVersion(VERSION);
        return;
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printHelp();
        return;
    }
    
    debug = false;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]).compare("-o") == 0)
        {
            if (i + 1 < argc)
                output = argv[++i];
            else
                throw Exception::ArgumentError("Missing output file after '-o'");
        }
        else if (std::string(argv[i]).compare("--arch") == 0)
        {
            if (i + 1 >= argc)
                throw Exception::ArgumentError("Missing output file after '--arch'");
            
            std::string archStr = toLower(argv[++i]);
            archStr = trim(archStr);

            if (archStr.compare("x86") == 0
             || archStr.compare("x86_64") == 0
             || archStr.compare("x86-64") == 0)
            {
                arch = Architecture::x86;
            }
            else if (archStr.compare("arm") == 0
                  || archStr.compare("aarch") == 0
                  || archStr.compare("aarch64") == 0)
            {
                arch = Architecture::ARM;
            }
            else if (archStr.compare("riscv") == 0
                  || archStr.compare("risc-v") == 0
                  || archStr.compare("risc_v") == 0)
            {
                arch = Architecture::RISC_V;
            }
            else
                throw Exception::ArgumentError("Unknown architecture: " + archStr);
        }
        else if (std::string(argv[i]).compare("--format") == 0)
        {
            if (i + 1 >= argc)
                throw Exception::ArgumentError("Missing output file after '--format'");
            
            std::string formatStr = toLower(argv[++i]);
            formatStr = trim(formatStr);

            if (formatStr.find("elf") == 0)
            {
                format = Format::ELF;
            }
            else if (formatStr.find("macho") == 0
                  || formatStr.find("mach_o") == 0
                  || formatStr.find("mach-o") == 0)
            {
                format = Format::MACHO;
            }
            else if (formatStr.find("coff") == 0
                  || formatStr.find("pe") == 0)
            {
                format = Format::COFF;
            }
            else
                throw Exception::ArgumentError("Unknown format: " + formatStr);
        }
        else if (std::string(argv[i]).compare("--endian") == 0)
        {
            if (i + 1 >= argc)
                throw Exception::ArgumentError("Missing output file after '--endian'");
            
            std::string endianStr = toLower(argv[++i]);
            endianStr = trim(endianStr);

            if (endianStr.find("little") == 0
             || endianStr.find("l") == 0
             || endianStr.find("lil") == 0
             || endianStr.find("le") == 0)
            {
                endianness = Endianness::Little;
            }
            else if (endianStr.find("big") == 0
                  || endianStr.find("b") == 0
                  || endianStr.find("be") == 0)
            {
                endianness = Endianness::Big;
            }
            else
                throw Exception::ArgumentError("Unknown Endianness: " + endianStr);
        }
        else if (std::string(argv[i]).find("-m") == 0)
        {
            std::string modeStr = std::string(argv[i]).substr(2);
            if (modeStr == "16") bits = BitMode::Bits16;
            else if (modeStr == "32") bits = BitMode::Bits32;
            else if (modeStr == "64") bits = BitMode::Bits64;
            else
                throw Exception::ArgumentError("Unknown bit mode: " + modeStr);
        }

        else if (std::string(argv[i]).find("--debug") == 0)
        {
            debug = true;
        }

        else if (!std::string(argv[i]).empty() && argv[i][0] == '-')
        {
            context.warningManager->add(Warning::ArgumentWarning("Unknown option: " + std::string(argv[i])));
        }
        else
        {
            input = argv[i];
        }
    }

    if (input.empty())
    {
        throw Exception::ArgumentError("No input file entered");
    }

    if (output.empty())
    {
        output = input + ".o";
    }
}