#pragma once

#include <fstream>
#include <string>
#include <vector>
#include <variant>
#include <Architecture.hpp>
#include "../Context.hpp"
#include "Tokenizer.hpp"

namespace Instruction
{
    struct Register
    {
        // TODO
    };

    enum class ImmediateType
    {
        Unsigned,
        Signed,
        Floating
    };

    struct Immediate
    {
        uint64_t value;
        ImmediateType type;
        uint16_t size;
    };

    struct Memory
    {
        // TODO
    };

    struct Label
    {
        std::string label;
    };

    using Operand = std::variant<Register, Immediate, Memory, Label>;

    struct Instruction
    {
        uint64_t mnemonic;
        std::vector<Operand> operands;
        BitMode bits;
        int alignment;

        size_t lineNumber;
        size_t column;
    };
}

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

    size_t lineNumber;
    size_t column;
};

using SectionEntry = std::variant<Instruction::Instruction, DataDefinition, Label>;

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
