#include "BinaryWriter.hpp"
#include <limits>
#include <memory>
#include <cstring>

Binary::Writer::Writer(const Context &_context, Architecture _arch, BitMode _bits, Format _format, std::ostream *_file, const Parser::Parser *_parser, const Encoder::Encoder *_encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{
}

void Binary::Writer::Write()
{
    std::vector<Encoder::Section> sections = encoder->getSections();

    std::vector<Encoder::Section> uninitialized;

    std::unordered_map<std::string, uint64_t> sectionOffsets;
    std::unordered_map<std::string, std::vector<uint8_t>*> sectionBuffers;

    uint64_t off = 0;
    for (Encoder::Section& section : sections)
    {
        if (section.align == 0)
            throw Exception::InternalError("Alignment not set for section '" + section.name + "'");

        uint64_t align = section.align;
        if (align < 4) align = 4; // TODO: minimal alignment of 4, check if useful

        off = (off + align - 1) / align * align;

        sectionBuffers[section.name] = &section.buffer;

        if (!section.isInitialized)
        {
            uninitialized.push_back(section);
            continue;
        }
        
        sectionOffsets[section.name] = off;

        off += static_cast<uint64_t>(section.size());
    }
    for (const Encoder::Section& section : uninitialized)
    {
        sectionOffsets[section.name] = off;

        uint64_t align = section.align;
        if (align < 4) align = 4; // TODO: minimal alignment of 4, check if useful

        off = (off + align - 1) / align * align;

        off += static_cast<uint64_t>(section.size());
    }

    for (const Encoder::Relocation& relocation : encoder->getRelocations())
    {
        auto itBuf = sectionBuffers.find(relocation.section);
        if (itBuf == sectionBuffers.end()) throw Exception::InternalError("Section wasn't found");
        std::vector<uint8_t>* sectionBuffer = itBuf->second;

        auto it = sectionOffsets.find(relocation.usedSection);
        if (it == sectionOffsets.end()) throw Exception::InternalError("Used section wasn't found");
        const uint64_t& sectionOffset = it->second;
        const int64_t value = sectionOffset + relocation.addend;

        const uint64_t& offset = relocation.offsetInSection;
        
        switch (relocation.type)
        {
            case Encoder::RelocationType::Absolute:
            {
                switch (relocation.size)
                {
                    case Encoder::RelocationSize::Bit8:
                    {
                        if (value < std::numeric_limits<int8_t>::min() ||
                            value > std::numeric_limits<int8_t>::max())
                            throw Exception::OverflowError("Relocation would overflow");
                        const int8_t val = static_cast<int8_t>(value);
                        std::memcpy(sectionBuffer->data() + offset, &val, sizeof(uint8_t));
                    }
                }
                break;
            }

            default: throw Exception::InternalError("Unknown relocation type"); break;
        }
    }

    // TODO
    for (const Encoder::Section& section : sections)
    {
        if (section.align == 0)
            throw Exception::InternalError("Alignment not set for section '" + section.name + "'");

        if (!section.isInitialized) continue;

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

    for (const Encoder::Section& section : uninitialized)
    {
        // TODO
    }
}