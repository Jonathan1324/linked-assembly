#pragma once

#include "../OutputWriter.hpp"

namespace Binary
{
    class Writer : public ::Output::Writer
    {
    public:
        Writer(const Context& _context, Architecture _arch, BitMode _bits, Format _format, std::ostream* _file, const Parser::Parser* _parser, const Encoder::Encoder* _encoder);
        ~Writer() = default;
        
        void Write() override;
    };
}