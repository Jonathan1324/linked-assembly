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

        Context context;
        Architecture arch;
        BitMode bits;

        const Parser::Parser* parser = nullptr;

        std::unordered_map<std::string, std::string> labelSection;
        std::unordered_map<std::string, size_t> labelOffset;

        std::vector<Section> sections;
    };

    Encoder* getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser::Parser* parser);
}
