#pragma once

#include <Architecture.hpp>
#include <vector>
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

        size_t size() const;
    };

    struct Label
    {
        std::string name;
        std::string section;
        size_t offset = 0;
    };

    class Encoder
    {
    public:
        Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser);
        virtual ~Encoder() = default;

        void Encode();
        void Print() const;

        std::vector<Section> getSections() const;
        
    protected:
        virtual std::vector<uint8_t> _EncodeInstruction(const Parser::Instruction::Instruction& instruction) = 0;
        virtual std::vector<uint8_t> _EncodePadding(size_t length) = 0;
        std::vector<uint8_t> _EncodeData(const Parser::DataDefinition& dataDefinition);

        uint64_t Evaluate(const Parser::Immediate& immediate) const;

        Context context;
        Architecture arch;
        BitMode bits;

        const Parser::Parser* parser = nullptr;

        std::vector<Section> sections;

        std::unordered_map<std::string, Label> labels;
        std::unordered_map<std::string, uint32_t> constants;

        size_t bytesWritten = 0;
        size_t sectionOffset = 0;
    };

    Encoder* getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser::Parser* parser);
}
