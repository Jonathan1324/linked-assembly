#include "BinaryWriter.hpp"

Output::Binary::Writer::Writer(const Context& _context, Architecture _arch, BitMode _bits, Format _format, std::ofstream& _file, const Parser::Parser* _parser, const Encoder::Encoder* _encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{

}

void Output::Binary::Writer::Write()
{
    const std::vector<Encoder::Section> sections = encoder->getSections();

    // TODO
    for (const auto& section : sections)
    {
        file.write(reinterpret_cast<const char*>(section.buffer.data()), section.buffer.size());
    }
}