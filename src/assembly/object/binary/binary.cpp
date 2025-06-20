#include "binary.hpp"

#include "util.hpp"

namespace Binary {
    Data create(BitMode bits, Architecture arch, Endianness endianness, Encoded encoded, Parsed parsed, Context& context)
    {
        Data data;

        std::unordered_map<std::string, uint64_t> labelOffsets;

        bool hasBSS = (encoded.sections.find(".bss") != encoded.sections.end());

        uint64_t offset = parsed.org;
        for (const auto& [name, section] : encoded.sections)
        {
            if (section.name.compare(".bss") == 0)
                continue;

            uint64_t align;
            if (bits == BitMode::Bits64)
                align = getAlignment64(section.name);
            else
                align = getAlignment32(section.name);
            size_t padding = (align - (offset % align)) % align;

            offset += padding;

            for (const auto& [labelName, label] : section.labels)
            {
                labelOffsets[label.name] = label.offset + offset;
                for (const auto& [localLabelName, localLabel] : label.localLabels)
                {
                    labelOffsets["@" + label.name + localLabel.name] = localLabel.offset + offset;
                }
            }
            offset += section.buffer.size();
        }

        if (hasBSS)
        {
            const auto& section = encoded.sections.at(".bss");

            //TODO: avoid duplicate code
            uint64_t align;
            if (bits == BitMode::Bits64)
                align = getAlignment64(section.name);
            else
                align = getAlignment32(section.name);
            size_t padding = (align - (offset % align)) % align;

            offset += padding;

            for (const auto& [labelName, label] : section.labels)
            {
                labelOffsets[label.name] = label.offset + offset;
                for (const auto& [localLabelName, localLabel] : label.localLabels)
                {
                    labelOffsets["@" + label.name + localLabel.name] = localLabel.offset + offset;
                }
            }
            offset += section.buffer.size();
        }

        offset = parsed.org;        
        for (const auto& [name, section] : encoded.sections)
        {
            sectionBuffer buffer = section.buffer;

            if (section.name.compare(".bss") == 0) 
                continue;

            uint64_t align;
            if (bits == BitMode::Bits64)
                align = getAlignment64(section.name);
            else
                align = getAlignment32(section.name);

            // add padding
            size_t padding = (align - (offset % align)) % align;
            data.buffer.insert(data.buffer.end(), padding, 0x00);

            offset += padding;

            for (const Relocation& relocation : section.relocations)
            {
                if (labelOffsets.find(relocation.labelName) == labelOffsets.end())
                    throw Exception::UndefinedSymbol("Label '" + relocation.labelName + "' not found");

                uint64_t labelOffset = labelOffsets.at(relocation.labelName);

                //TODO
                std::cout << "Relocation: label: " << relocation.labelName
                          << ", addend: " << relocation.addend
                          << ", offset in section: " << relocation.offsetInSection
                          << ", size: " << (uint64_t)relocation.size
                          << ", type: " << (uint64_t)relocation.type
                          << std::endl;

                if (relocation.size == 8)
                {
                    switch (relocation.type)
                    {
                        case Type::Absolute:
                            buffer.at(relocation.offsetInSection) = labelOffset + relocation.addend;
                            break;
                        
                        default:
                            throw Exception::InternalError("Unknown relocation type");
                    }
                }
                else if (relocation.size == 16)
                {
                    //TODO
                    throw Exception::InternalError("Unknown relocation size");
                }
                else if (relocation.size == 32)
                {
                    //TODO
                    throw Exception::InternalError("Unknown relocation size");
                }
                else if (relocation.size == 64)
                {
                    //TODO
                    throw Exception::InternalError("Unknown relocation size");
                }
                else
                    throw Exception::InternalError("Unknown relocation size");
            }

            data.buffer.insert(data.buffer.end(), buffer.begin(), buffer.end());
            offset += buffer.size();
        }

        if (hasBSS)
        {
            const auto& section = encoded.sections.at(".bss");

            //TODO: avoid duplicate code
            sectionBuffer buffer = section.buffer;

            for (const Relocation& relocation : section.relocations)
            {
                if (labelOffsets.find(relocation.labelName) == labelOffsets.end())
                    throw Exception::UndefinedSymbol("Label '" + relocation.labelName + "' not found");

                uint64_t labelOffset = labelOffsets.at(relocation.labelName);

                //TODO
                std::cout << "Relocation: label: " << relocation.labelName
                          << ", addend: " << relocation.addend
                          << ", offset in section: " << relocation.offsetInSection
                          << ", size: " << (uint64_t)relocation.size
                          << ", type: " << (uint64_t)relocation.type
                          << std::endl;

                if (relocation.size == 8)
                {
                    switch (relocation.type)
                    {
                        case Type::Absolute:
                            buffer.at(relocation.offsetInSection) = labelOffset + relocation.addend;
                            break;
                        
                        default:
                            throw Exception::InternalError("Unknown relocation type");
                    }
                }
                else if (relocation.size == 16)
                {
                    //TODO
                    throw Exception::InternalError("Unknown relocation size");
                }
                else if (relocation.size == 32)
                {
                    //TODO
                    throw Exception::InternalError("Unknown relocation size");
                }
                else if (relocation.size == 64)
                {
                    //TODO
                    throw Exception::InternalError("Unknown relocation size");
                }
                else
                    throw Exception::InternalError("Unknown relocation size");
            }
        }

        return data;
    }

    void write(std::ofstream& out, Endianness endianness, Data& data, Context& context)
    {
        out.write(reinterpret_cast<const char*>(data.buffer.data()), data.buffer.size());
    }
}