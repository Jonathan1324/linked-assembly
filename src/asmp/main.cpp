#include <iostream>
#include <fstream>

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

    try
    {
        bool stop = parseArguments(argc, argv, inputFile, outputFile, context);
        if (stop)
            return 0;

        if (warningManager.hasWarnings())
        {
            warningManager.printAll(std::cerr);
            warningManager.clear();
        }

        std::ofstream output = openOfstream(outputFile, std::ios::out | std::ios::trunc);
        std::ifstream input = openIfstream(inputFile);

        PreProcessor preprocessor(context, output, input);
        preprocessor.Process();

        input.close();
        output.close();
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