#pragma once

#include <Architecture.hpp>
#include <vector>
#include <IntTypes.h>
#include <unordered_set>
#include "../Context.hpp"
#include "../Parser/Parser.hpp"

namespace Encoder
{
    using SectionBuffer = std::vector<uint8_t>;

    struct Section
    {
        std::string name;
        bool isInitialized = true;
        SectionBuffer buffer;
        size_t reservedSize = 0;

        uint64_t align;

        size_t size() const;
    };

    struct Label
    {
        std::string name;
        std::string section;
        uint64_t offset = 0;

        bool isGlobal;
        bool resolved;
    };

    enum class HasPos
    {
        UNKNOWN,
        TRUE,
        FALSE
    };

    struct Constant
    {
        std::string name;
        Parser::Immediate expression;
        HasPos hasPos;
        int64_t value;

        size_t offset;
        size_t bytesWritten;

        bool resolved;
        bool prePass = false;
    };
    

    class Encoder
    {
    public:
        Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser);
        virtual ~Encoder() = default;

        void Encode();
        void Optimize();
        void Print() const;

        const std::vector<Section>& getSections() const { return sections; };
        const std::unordered_map<std::string, Label>& getLabels() const { return labels; };
        const std::unordered_map<std::string, Constant>& getConstants() const { return constants; };
        
    protected:
        virtual std::vector<uint8_t> EncodeInstruction(const Parser::Instruction::Instruction& instruction, bool ignoreUnresolved = false) = 0;
        virtual uint64_t GetSize(const Parser::Instruction::Instruction& instruction) = 0;
        virtual std::vector<uint8_t> EncodePadding(size_t length) = 0;
        std::vector<uint8_t> EncodeData(const Parser::DataDefinition& dataDefinition);
        uint64_t GetSize(const Parser::DataDefinition& dataDefinition);

        Int128 Evaluate(const Parser::Immediate& immediate, uint64_t bytesWritten, uint64_t sectionOffset) const;

        void resolveConstants(bool withPos);
        bool Resolvable(const Parser::Immediate& immediate);
        std::vector<std::string> getDependencies(const Parser::Immediate& immediate);
        bool resolveConstantWithoutPos(Constant& c, std::unordered_set<std::string>& visited);
        bool resolveConstantWithPos(Constant& c, std::unordered_set<std::string>& visited);

        Context context;
        Architecture arch;
        BitMode bits;

        const Parser::Parser* parser = nullptr;

        std::vector<Section> sections;

        std::unordered_map<std::string, Label> labels;
        std::unordered_map<std::string, Constant> constants;

        size_t bytesWritten = 0;
        size_t sectionOffset = 0;
    };

    Encoder* getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser::Parser* parser);
}
