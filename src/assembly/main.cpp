#include <iostream>
#include <fstream>

#include "parser.hpp"
#include "symbol.hpp"
#include "encoder/encoder.hpp"
#include "Architecture.hpp"
#include "object/object.hpp"

#include "util/string.hpp"
#include <cstring>

#include "debug.hpp"

#include "cli/version.h"
#include "cli/help.h"

#include "Exception.hpp"

int main(int argc, const char *argv[])
{
    std::string input_path;
    std::string output_path;
    BitMode bitMode;
    Architecture arch;
    Format format;
    Endianness endianness;
    bool debug;

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

    Encoded encoded = encode(parsed, arch, endianness);

    if (debug)
        printEncoded(encoded);

    createFile(format, objectFile, bitMode, arch, encoded, parsed, endianness, debug);

    objectFile.close();

    return 0;
}