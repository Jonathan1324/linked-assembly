#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <Architecture.hpp>
#include "../Context.hpp"
#include "Tokenizer.hpp"

struct Instruction
{

};

struct DataDefinition
{
    size_t size;
    bool reserved;
    std::vector<uint64_t> values;
    int alignment;

    size_t lineNumber;
    size_t column;
};

struct Label
{
    std::string name;
    bool isGlobal;
};

using SectionEntry = std::variant<Instruction, DataDefinition, Label>;

struct Section
{
    std::string name;
    std::vector<SectionEntry> entries;
};

class Parser
{
public:
    Parser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness);
    virtual ~Parser() = default;

    virtual void Parse(const std::vector<Token::Token>& tokens) = 0;
    void Print();

protected:
    Context context;
    Architecture arch;
    BitMode bits;
    Endianness endianness;

    uint64_t org = 0;
    std::vector<Section> sections;
    std::vector<std::string> externs;
};

// FIXME: only temporary solution
Parser* getParser(Context _context, Architecture _arch, BitMode _bits, Endianness _endianness);
