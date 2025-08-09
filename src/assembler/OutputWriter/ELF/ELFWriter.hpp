#pragma once

#include "../OutputWriter.hpp"
#include "Headers.hpp"

namespace ELF
{
    struct Section
    {
        const std::vector<uint8_t>* buffer;

        bool writeBuffer = true;
        
        std::variant<SectionHeader32, SectionHeader64> header;

        bool nullSection = false;
    };

    class Writer : public Output::Writer
    {
    public:
        Writer(const Context& _context, Architecture _arch, BitMode _bits, Format _format, std::ostream* _file, const Parser::Parser* _parser, const Encoder::Encoder* _encoder);
        ~Writer() = default;

        void Write() override;

    protected:
        std::vector<Section> sections;

        using SymbolEntry = std::variant<Symbol::Entry32, Symbol::Entry64>;
        std::vector<SymbolEntry> localSymbols;
        std::vector<SymbolEntry> globalSymbols;
        std::vector<SymbolEntry> weakSymbols;
    };
}
