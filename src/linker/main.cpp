#include <iostream>
#include <vector>
#include <filesystem>
#include <fstream>
#include <string>

#include "architecture/architecture.hpp"

#include "cli/help.h"
#include "arguments.hpp"
#include "Context.hpp"

#include <Exception.hpp>
#include <io/file.hpp>

int handleError(const std::exception& e) {
    std::cerr << e.what() << std::endl;
    return 1;
}

int main(int argc, const char *argv[])
{
    WarningManager warningManager;
    Context context;
    context.warningManager = &warningManager;

    std::vector<std::string> inputFiles;
    std::string output;
    BitMode bitMode;
    Architecture arch;
    Format format;
    bool debug;

    // Parse arguments
    try
    {
        bool stop = parseArguments(argc, argv, inputFiles, output, bitMode, arch, format, debug, context);
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
    catch(const std::exception& e) { handleError(e); }

    for (const auto& inputFile : inputFiles)
    {
        context.filename = std::filesystem::path(inputFile).filename().string();

        std::istream* file = nullptr;
        try
        {
            file = openIstream(inputFile);
        }
        catch(const Exception& e)
        {
            e.print(std::cerr);
            return 1;
        }
        catch(const std::exception& e) { handleError(e); }

        char buffer[4];

        file->read(buffer, 4);

        Format format = getFormat(buffer);

        if (inputFile != "-")
            delete file;
    }

    std::ostream* objectFile = nullptr;
    try
    {
        objectFile = openOstream(output, std::ios::out | std::ios::trunc | std::ios::binary);
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        return 1;
    }
    catch(const std::exception& e) { handleError(e); }

    std::cout << "Linker not implemented yet. ):" << std::endl;

    if (output != "-")
        delete objectFile;

    return 0;
}