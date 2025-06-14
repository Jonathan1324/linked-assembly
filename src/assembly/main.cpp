#include <iostream>
#include <fstream>

#include "parser.hpp"
#include "symbol.hpp"
#include "encoder/encoder.hpp"
#include "architecture/architecture.hpp"
#include "object/object.hpp"

#include "util/string.hpp"
#include <cstring>

#include "debug.hpp"

#include "cli/version.h"
#include "cli/help.h"

constexpr const char* version = "v0.1.0-alpha";

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: assembly <input.asm> [-o output.o]" << std::endl;
        return 1;
    }

    if (strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0)
    {
        printVersion(version);
        return 0;
    }
    else if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
    {
        printHelp();
        return 0;
    }
    
    std::string input_path = "";
    std::string output_path = "";

    BitMode bitMode = BitMode::Bits64; // Default
    #ifdef __x86_64__
        // 64 bit - bitMode = BitMode::Bits64;
    #elif defined(__i386__)
        bitMode = BitMode::Bits32;
    #elif defined(__arm__)
        bitMode = BitMode::Bits32;
    #elif defined(__aarch64__)
        // 64 bit - bitMode = BitMode::Bits64;
    #endif

    Architecture arch = Architecture::x86; // Default
    #if defined(__aarch64__) || defined(__arm__) || defined(_M_ARM)
        arch = Architecture::ARM;
    #elif defined(__riscv)
        arch = Architecture::RISC_V;
    #endif

    Format format = Format::ELF;
    #if defined(_WIN32) || defined(_WIN64)
        format = Format::COFF;
    #elif defined(__APPLE__) && defined(__MACH__)
        format = Format::MACHO;
    #elif defined(__linux__)
        // ELF - format = Format::ELF;
    #elif defined(__unix__)
        // ELF - format = Format::ELF;
    #endif


    bool debug = false;

    for (int i = 1; i < argc; ++i)
    {
        if (std::string(argv[i]).compare("-o") == 0 && i + 1 < argc)
        {
            if (i + 1 < argc)
            {
                output_path = argv[++i];
            }
            else
            {
                std::cerr << "Error: Missing output file after -o" << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]).compare("--arch") == 0 && i + 1 < argc)
        {
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
            {
                std::cerr << "Unknown architecture: " << archStr << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]).compare("--format") == 0 && i + 1 < argc)
        {
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
            {
                std::cerr << "Unknown Format: " << formatStr << std::endl;
                return 1;
            }
        }
        else if (std::string(argv[i]).find("-m") == 0)
        {
            std::string modeStr = std::string(argv[i]).substr(2);
            if (modeStr == "16") bitMode = BitMode::Bits16;
            else if (modeStr == "32") bitMode = BitMode::Bits32;
            else if (modeStr == "64") bitMode = BitMode::Bits64;
            else {
                std::cerr << "Unknown bit mode: " << modeStr << std::endl;
                return 1;
            }
        }

        else if (std::string(argv[i]).find("--debug") == 0)
        {
            debug = true;
        }

        else if (!std::string(argv[i]).empty() && argv[i][0] == '-')
        {
            std::cerr << "Warning: Unknown option " << argv[i] << std::endl;
        }
        else
        {
            input_path = argv[i];
        }
    }

    if (input_path.empty())
    {
        std::cerr << "Enter input file" << std::endl;
        return 1;
    }

    if (output_path.empty())
    {
        output_path = input_path + ".o";
    }

    std::ifstream file(input_path);
    if (!file.is_open())
    {
        std::cerr << "Couldn't open file " << input_path << std::endl;
        return 1;
    }

    Parsed parsed = parseAssembly(file, bitMode);

    file.close();

    resolveParsed(parsed);

    if (debug)
        printParsed(parsed);

    std::ofstream objectFile(output_path, std::ios::out | std::ios::trunc);
    if (!objectFile)
    {
        std::cerr << "Couldn't open file " << output_path << std::endl;
        return 1;
    }

    Encoded encoded = encode(parsed, arch);

    if (debug)
        printEncoded(encoded);

    createFile(format, objectFile, bitMode, arch, encoded, parsed, debug);

    objectFile.close();

    return 0;
}