#include "BinaryWriter.hpp"

Output::Binary::Writer::Writer(const Context &_context, Architecture _arch, BitMode _bits, Format _format, std::ostream *_file, const Parser::Parser *_parser, const Encoder::Encoder *_encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{
}

void Output::Binary::Writer::Write()
{
    const std::vector<Encoder::Section> sections = encoder->getSections();

    std::vector<Encoder::Section> uninitialized;

    // TODO
    for (const auto &section : sections)
    {
        if (section.align == 0)
            throw Exception::InternalError("Alignment not set for section '" + section.name + "'");

        if (!section.isInitialized)
        {
            uninitialized.push_back(section);
            continue;
        }

        std::streampos pos = file->tellp();
        if (pos == -1)
            throw Exception::IOError("Failed to get file position");

        uint64_t offset = static_cast<uint64_t>(pos);

        uint64_t padding = (section.align - (offset % section.align)) % section.align;

        if (padding > 0)
        {
            std::vector<char> pad(padding, 0);
            file->write(pad.data(), pad.size());
        }

        file->write(reinterpret_cast<const char *>(section.buffer.data()), section.buffer.size());
    }

    for (const auto &section : uninitialized)
    {
        // TODO
    }
}