#include "OutputWriter.hpp"

using namespace Output;

Output::Writer::Writer(const Context& _context, Architecture _arch, BitMode _bits, Format _format, std::ofstream& _file, const Parser::Parser* _parser, const Encoder::Encoder* _encoder)
    : context(_context), arch(_arch), bits(_bits), format(_format), file(_file), parser(_parser), encoder(_encoder)
{

}

void Output::Writer::Print() const
{
    // TODO
}

#include "binary/BinaryWriter.hpp"

// FIXME: only temporary solution
Output::Writer* Output::getWriter(const Context& context, Architecture arch, BitMode bits, Format format, std::ofstream& file, const Parser::Parser* parser, const Encoder::Encoder* encoder)
{
    switch(format)
    {
        case Format::Binary: return new Binary::Writer(context, arch, bits, format, file, parser, encoder);
        
        default: return nullptr;
    }
}