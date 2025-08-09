#include "BinaryWriter.hpp"

Binary::Writer::Writer(const Context &_context, Architecture _arch, BitMode _bits, Format _format, std::ostream *_file, const Parser::Parser *_parser, const Encoder::Encoder *_encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{
}

void Binary::Writer::Write()
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

        uint64_t align = section.align;
        if (align < 4) align = 4; // TODO: minimal alignment of 4, check if useful

        uint64_t padding = (align - (offset % align)) % align;

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