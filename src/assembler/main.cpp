#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include <io/file.hpp>
#include <Architecture.hpp>
#include <Exception.hpp>
#include "arguments.hpp"
#include "Context.hpp"

int handleError(const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return 1;
}

int main(int argc, const char *argv[])
{
    WarningManager warningManager;
    Context context;
    context.warningManager = &warningManager;

    std::string input_path;
    std::string output_path;
    BitMode bitMode;
    Architecture arch;
    Format format;
    Endianness endianness;
    bool debug;

    // Parse arguments
    try
    {
        bool stop = parseArguments(argc, argv, input_path, output_path, bitMode, arch, format, endianness, debug, context);
        if (stop)
            return 0;
        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e) { return handleError(e); }

    context.filename = std::filesystem::path(input_path).filename().string();
    
    // create file handles
    std::ifstream file;
    std::ofstream objectFile;
    try
    {
        file = openIfstream(input_path);
        objectFile = openOfstream(output_path, std::ios::out | std::ios::trunc | std::ios::binary);
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e) { return handleError(e); }

    // Parse

    // Encode

    // Create .o/.bin file

    return 0;
}