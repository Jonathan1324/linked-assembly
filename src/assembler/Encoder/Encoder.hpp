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
        bool isExtern = false;
        bool externUsed = false;
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
        std::string section;
        Parser::Immediate expression;
        int64_t value;

        int64_t off;
        std::string usedSection;

        HasPos hasPos;
        bool useOffset = false;

        size_t offset;
        size_t bytesWritten;

        bool isGlobal;
        bool resolved;
        bool prePass = false;
        bool relocationPossible = false;
    };

    enum class RelocationType
    {
        Absolute
    };

    enum class RelocationSize
    {
        Bit8 = 8,
        Bit16 = 16,
        Bit24 = 24,
        Bit32 = 32,
        Bit64 = 64
    };

    struct Relocation
    {
        uint64_t offsetInSection;
        int64_t addend;
        std::string section;
        std::string usedSection;

        RelocationType type;
        RelocationSize size;

        bool addendInCode = false;
        bool isExtern = false;
    };


    struct Evaluation
    {
        Int128 result;
        int64_t offset;
        bool useOffset;
        bool relocationPossible;
        bool isExtern;

        std::string usedSection;
    };
    
    

    class Encoder
    {
    public:
        Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser);
        virtual ~Encoder() = default;

        void Encode();
        void Optimize();
        void Print() const;

        using Symbol = std::variant<Label*, Constant*>;
        const std::vector<Section>& getSections() const { return sections; };
        const std::vector<Symbol>& getSymbols() const { return symbols; };
        const std::vector<Relocation>& getRelocations() const { return relocations; }
        
    protected:
        void EncodeFinal(std::vector<Parser::Section>& parsedSections);
        void GetOffsets(std::vector<Parser::Section>& parsedSections);
        void ResolveConstantsPrePass(const std::vector<Parser::Section>& parsedSections);

        virtual bool OptimizeOffsets(std::vector<Parser::Section>& parsedSections) = 0;

        virtual std::vector<uint8_t> EncodeInstruction(Parser::Instruction::Instruction& instruction, bool ignoreUnresolved = false, bool optimize = false) = 0;
        virtual uint64_t GetSize(Parser::Instruction::Instruction& instruction) = 0;
        virtual std::vector<uint8_t> EncodePadding(size_t length) = 0;
        std::vector<uint8_t> EncodeData(const Parser::DataDefinition& dataDefinition);
        uint64_t GetSize(const Parser::DataDefinition& dataDefinition);

        Evaluation Evaluate(const Parser::Immediate& immediate, uint64_t bytesWritten, uint64_t sectionOffset, const std::string* curSection);

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
        std::vector<Relocation> relocations;

        std::unordered_map<std::string, uint64_t> sectionStarts;
        std::unordered_map<std::string, Label> labels;
        std::unordered_map<std::string, Constant> constants;

        std::vector<Symbol> symbols;

        size_t bytesWritten = 0;
        size_t sectionOffset = 0;
        const std::string* currentSection;
    };

    Encoder* getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser::Parser* parser);
}
