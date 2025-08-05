#include "BinaryWriter.hpp"

Output::Binary::Writer::Writer(const Context& _context, Architecture _arch, BitMode _bits, Format _format, std::ostream* _file, const Parser::Parser* _parser, const Encoder::Encoder* _encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{

}

void Output::Binary::Writer::Write()
{
    const std::vector<Encoder::Section> sections = encoder->getSections();

    std::vector<Encoder::Section> uninitialized;

    // TODO
    for (const auto& section : sections)
    {
        if (!section.isInitialized)
        {
            uninitialized.push_back(section);
            continue;
        }

        file->write(reinterpret_cast<const char*>(section.buffer.data()), section.buffer.size());
    }

    for (const auto& section : uninitialized)
    {
        // TODO
    }
}