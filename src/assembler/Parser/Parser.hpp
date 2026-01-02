#pragma once

#include <fstream>
#include <string>
#include <variant>
#include <unordered_map>
#include <Architecture.hpp>
#include <vector>
#include "../Context.hpp"
#include "Tokenizer.hpp"

namespace Parser
{
    struct Integer
    {
        uint64_t value;
    };

    struct Operator
    {
        std::string op;
    };

    struct String
    {
        std::string value;
    };

    struct CurrentPosition
    {
        bool sectionPos;
    };

    using ImmediateOperand = std::variant<Integer, Operator, String, CurrentPosition>;

    struct Immediate
    {
        std::vector<ImmediateOperand> operands;
    };

    namespace Instruction
    {
        struct Register
        {
            uint64_t reg;
        };

        struct Memory
        {
            // TODO
        };

        using Operand = std::variant<Register, Immediate, Memory>;

        struct Instruction
        {
            uint64_t mnemonic;
            std::vector<Operand> operands;
            BitMode bits;

            size_t lineNumber;
            size_t column;

            Instruction(uint64_t _mnemonic, BitMode _bits, size_t _lineNumber = 0, size_t _column = 0)
                : mnemonic(_mnemonic), bits(_bits), lineNumber(_lineNumber), column(_column) {}
        };
    }

    struct DataDefinition
    {
        size_t size;
        bool reserved;
        std::vector<Immediate> values;

        size_t lineNumber;
        size_t column;
    };

    struct Label
    {
        std::string name;
        bool isGlobal;
        bool isExtern;

        size_t lineNumber;
        size_t column;
    };

    struct Constant
    {
        std::string name;
        Immediate value;
        bool isGlobal;

        bool hasPos;

        size_t lineNumber;
        size_t column;
    };

    struct Repetition
    {
        Immediate count;

        size_t lineNumber;
        size_t column;
    };

    struct Alignment
    {
        Immediate align;

        size_t lineNumber;
        size_t column;
    };

    using SectionEntry = std::variant<Instruction::Instruction, DataDefinition, Label, Constant, Repetition, Alignment>;

    struct Section
    {
        std::string name;
        std::vector<SectionEntry> entries;

        uint64_t align = 0;
    };

    class Parser
    {
    public:
        Parser(const Context& _context, Architecture _arch, BitMode _bits);
        virtual ~Parser() = default;

        virtual void Parse(const std::vector<Token::Token>& tokens) = 0;
        void Print() const;

        const std::string& getOrg() const noexcept { return org; }
        const std::vector<Section>& getSections() const noexcept { return sections; }

    protected:
        Context context;
        Architecture arch;
        BitMode bits;

        std::string org;
        std::vector<Section> sections;
    };

    Parser* getParser(const Context& context, Architecture arch, BitMode bits);
}
