#include "Arguments.hpp"

#include <cstring>
#include <Exception.hpp>
#include <version.h>
#include <util/string.hpp>

void printHelp(const char* name, std::ostream& s)
{
    s << "Usage: " << name << "<inputs> (-o <output>) (--arch <x86>) (--format <bin/elf>) (--bits <16/32/64>) (--debug) (--no-preprocess)" << std::endl;

    s << std::endl << "Flags:" << std::endl;
    s << "> --arch <arch>             Set architecture" << std::endl;
    s << "> --format <format>         Set output format" << std::endl;
    s << "> --bits <16/32/64>         Set bit mode" << std::endl;
    s << "> --debug                   Print debug information" << std::endl;
    s << "> --no-preprocess           Don't execute the preprocessor" << std::endl;
    
}

bool parseArguments(int argc, const char *argv[],
                    std::vector<std::string>& inputs, std::string& output,
                    BitMode& bits, Architecture& arch, Format& format,
                    bool& debug, bool& preprocess, Context& context)
{
#ifdef __x86_64__
    bits = BitMode::Bits64;
#elif defined(__i386__)
    bits = BitMode::Bits32;
#elif defined(__arm__)
    bits = BitMode::Bits32;
#elif defined(__aarch64__)
    bits = BitMode::Bits64;
#else
    bits = BitMode::Bits64;
#endif

#if defined(__aarch64__) || defined(__arm__) || defined(_M_ARM)
    arch = Architecture::ARM;
#elif defined(__riscv)
    arch = Architecture::RISC_V;
#else
    arch = Architecture::x86;
#endif

#if defined(_WIN32) || defined(_WIN64)
    format = Format::COFF;
#elif defined(__APPLE__) && defined(__MACH__)
    format = Format::MACHO;
#elif defined(__linux__)
    format = Format::ELF;
#elif defined(__unix__)
    format = Format::ELF;
#else
    format = Format::ELF;
#endif

    if (argc < 2)
    {
        throw Exception::ArgumentError("No input file specified", -1, -1, "command-line");
    }

    if (std::strcmp(argv[1], "--version") == 0 || std::strcmp(argv[1], "-v") == 0)
    {
        printVersion();
        return true;
    }
    else if (std::strcmp(argv[1], "--help") == 0 || std::strcmp(argv[1], "-h") == 0)
    {
        printHelp(argv[0], std::cout);
        return true;
    }
    
    debug = false;
    preprocess = true;

    for (int i = 1; i < argc; ++i)
    {
        if (std::strcmp(argv[i], "-o") == 0)
        {
            if (i + 1 < argc)
                output = argv[++i];
            else
                throw Exception::ArgumentError("Missing output file after '-o'", -1, -1, "command-line");
        }
        else if (std::strcmp(argv[i], "--arch") == 0)
        {
            if (i + 1 >= argc)
                throw Exception::ArgumentError("Missing arg after '--arch'", -1, -1, "command-line");
            
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
                throw Exception::ArgumentError("Unknown architecture: " + archStr, -1, -1, "command-line");
        }
        else if (std::strcmp(argv[i], "--format") == 0)
        {
            if (i + 1 >= argc)
                throw Exception::ArgumentError("Missing arg after '--format'", -1, -1, "command-line");
            
            std::string formatStr = toLower(argv[++i]);
            formatStr = trim(formatStr);

            if (formatStr.find("bin") == 0
             || formatStr.find("raw") == 0)
            {
                format = Format::Binary;
            }
            else if (formatStr.find("elf") == 0)
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
                throw Exception::ArgumentError("Unknown format: " + formatStr, -1, -1, "command-line");
        }
        else if (std::strcmp(argv[i], "--bits") == 0)
        {
            if (i + 1 >= argc)
                throw Exception::ArgumentError("Missing arg after '--format'", -1, -1, "command-line");
            
            std::string bitsStr = toLower(argv[++i]);
            bitsStr = trim(bitsStr);

            if (bitsStr == "16") bits = BitMode::Bits16;
            else if (bitsStr == "32") bits = BitMode::Bits32;
            else if (bitsStr == "64") bits = BitMode::Bits64;
            else
                throw Exception::ArgumentError("Unknown bit mode: " + bitsStr, -1, -1, "command-line");
        }

        else if (std::strcmp(argv[i], "--debug") == 0 || std::strcmp(argv[i], "-d") == 0)
        {
            debug = true;
        }
        else if (std::strcmp(argv[i], "--no-preprocess") == 0)
        {
            preprocess = false;
        }

        else if (argv[i][0] == '-' && argv[i][1] != '\0')
        {
            context.warningManager->add(Warning::ArgumentWarning("Unknown option: " + std::string(argv[i])));
        }
        else
        {
            inputs.push_back(argv[i]);
        }
    }

    if (inputs.empty())
    {
        throw Exception::ArgumentError("No input file entered", -1, -1, "command-line");
    }

    if (output.empty())
    {
        if (format == Format::Binary)
            output = inputs.at(0) + ".bin";
        else
            output = inputs.at(0) + ".o";
    }

    return false;
}
