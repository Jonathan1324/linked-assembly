#pragma once

#include <Architecture.hpp>
#include "../Context.hpp"
#include "../Parser/Parser.hpp"
#include "../Encoder/Encoder.hpp"

namespace Output
{
    class Writer
    {
    public:
        Writer(const Context& _context, Architecture _arch, BitMode _bits, Format _format, std::ostream* _file, const Parser::Parser* _parser, const Encoder::Encoder* _encoder);
        virtual ~Writer() = default;

        virtual void Write() = 0;
        void Print() const;

    protected:
        Context context;
        Architecture arch;
        BitMode bits;
        Format format;
        std::ostream* file;

        const Parser::Parser* parser = nullptr;
        const Encoder::Encoder* encoder = nullptr;
    };

    Writer* getWriter(const Context& context, Architecture arch, BitMode bits, Format format, std::ostream* file, const Parser::Parser* parser, const Encoder::Encoder* encoder);
}