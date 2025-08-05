#pragma once

#include <fstream>
#include "Context.hpp"

class PreProcessor
{
public:
    PreProcessor(const Context& _context, std::ofstream& _output, std::ifstream& _input);
    ~PreProcessor() = default;

    void Process();

private:
    const Context& context;
    std::ofstream& output;
    std::ifstream& input;
};