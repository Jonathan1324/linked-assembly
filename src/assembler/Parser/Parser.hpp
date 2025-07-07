#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <Architecture.hpp>
#include "../Context.hpp"

struct Instruction
{
    std::string mnemonic;
    std::vector<std::string> operands;

    BitMode mode;
    int alignment;
    int lineNumber;
};

struct DataDefinition
{
    std::string type;
    std::vector<std::string> values;
    bool reserved;

    int alignment;
    int lineNumber;
};

struct Label
{
    std::string name;

    int lineNumber;
};

struct Directive
{
    std::string directive;
    std::string argument;

    int lineNumber;
};

using SectionEntry = std::variant<Instruction, DataDefinition, Label, Directive>;
struct Section
{
    std::string name;
    std::vector<SectionEntry> entries;
};

class Parser
{
public:
    Parser(std::istream& _input, Context _context, Architecture _arch, BitMode _bits, Endianness _endianness);

    void Parse();

    void Print();

private:
    std::istream& input;
    Context context;

    Architecture arch;
    BitMode bits;
    Endianness endianness;

    std::vector<Section> sections;
};