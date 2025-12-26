#include <iostream>
#include <fstream>
#include <filesystem>

#include <io/file.hpp>
#include <Exception.hpp>
#include "cli/Arguments.hpp"
#include "Context.hpp"

#include "Preprocessor.hpp"

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

    std::string inputFile;
    std::string outputFile;
    bool debug;

    try
    {
        bool stop = parseArguments(argc, argv, inputFile, outputFile, debug, context);
        if (stop)
            return 0;

        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }

        std::ostream* output = openOstream(outputFile, std::ios::out | std::ios::trunc);
        std::istream* input = openIstream(inputFile);

        context.filename = inputFile;
        context.include_paths.clear();
        
        std::filesystem::path inputPath(inputFile);
        
        context.include_paths.push_back(inputPath.parent_path());

        PreProcessor preprocessor(context);
        preprocessor.Process(output, input, context.filename);

        if (debug)
            preprocessor.Print();

        if (inputFile != "-")
            delete input;
        if (outputFile != "-")
            delete output;
    }
    catch(const Exception& e)
    {
        e.print(std::cerr);
        std::remove(outputFile.c_str());
        return 1;
    }
    catch(const std::exception& e)
    {
        std::remove(outputFile.c_str());
        return handleError(e);
    }

    return 0;
}