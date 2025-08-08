#include "ELFWriter.hpp"

ELF::Writer::Writer(const Context &_context, Architecture _arch, BitMode _bits, Format _format, std::ostream *_file, const Parser::Parser *_parser, const Encoder::Encoder *_encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{
}

void ELF::Writer::Write()
{
    constexpr std::streamoff alignTo = 0x10;

    const std::vector<Encoder::Section>& eSections = encoder->getSections();

    Section nullSection;
    nullSection.writeBuffer = false;
    nullSection.nullSection = true;
    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
    {
        SectionHeader32 header;
        header.OffsetInSectionNameStringTable = 0;
        header.Type = 0;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.Offset = 0;
        header.SectionSize = 0;
        header.LinkIndex = 0;
        header.Info = 0;
        header.AddressAlignment = 0;
        header.EntrySize = 0;
        
        nullSection.header = header;
    }
    else if (bits == BitMode::Bits64)
    {
        SectionHeader64 header;
        header.OffsetInSectionNameStringTable = 0;
        header.Type = 0;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.Offset = 0;
        header.SectionSize = 0;
        header.LinkIndex = 0;
        header.Info = 0;
        header.AddressAlignment = 0;
        header.EntrySize = 0;

        nullSection.header = header;
    }
    else throw Exception::InternalError("Unknown bit mode");

    sections.push_back(std::move(nullSection));

    std::vector<uint8_t> shstrtabBuffer;
    shstrtabBuffer.push_back(0);

    for (const auto& section : eSections)
    {
        Section s;
        s.buffer = &section.buffer;

        uint32_t nameOffset = static_cast<uint32_t>(shstrtabBuffer.size());
        shstrtabBuffer.insert(shstrtabBuffer.end(), section.name.begin(), section.name.end());
        shstrtabBuffer.push_back(0);

        if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
        {
            SectionHeader32 header;
            header.OffsetInSectionNameStringTable = nameOffset;
            header.Type = SectionType::ProgBits;    // TODO
            header.Flags = 0;                       // TODO
            header.VirtualAddress = 0;
            header.Offset;  // TODO: set when writing
            if (section.isInitialized)
                header.SectionSize = static_cast<uint32_t>(section.buffer.size());
            else
                header.SectionSize = static_cast<uint32_t>(section.reservedSize);
            header.LinkIndex = 0;                   // TODO
            header.Info = 0;                        // TODO
            header.AddressAlignment = section.align;
            header.EntrySize = 0;
            
            s.header = header;
        }
        else if (bits == BitMode::Bits64)
        {
            SectionHeader64 header;
            header.OffsetInSectionNameStringTable = nameOffset;
            header.Type = SectionType::ProgBits;    // TODO
            header.Flags = 0;                       // TODO
            header.VirtualAddress = 0;
            header.Offset;  // TODO: set when writing
            if (section.isInitialized)
                header.SectionSize = static_cast<uint64_t>(section.buffer.size());
            else
                header.SectionSize = static_cast<uint64_t>(section.reservedSize);
            header.LinkIndex = 0;                   // TODO
            header.Info = 0;                        // TODO
            header.AddressAlignment = section.align;
            header.EntrySize = 0;

            s.header = header;
        }
        else throw Exception::InternalError("Unknown bit mode");

        sections.push_back(std::move(s));
    }

    // SHSTRTAB
    Section shstrtab;
    shstrtab.buffer = &shstrtabBuffer;
    std::string shstrtabName = ".shstrtab";

    uint32_t nameOffset = static_cast<uint32_t>(shstrtabBuffer.size());
    shstrtabBuffer.insert(shstrtabBuffer.end(), shstrtabName.begin(), shstrtabName.end());
    shstrtabBuffer.push_back(0);

    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
    {
        SectionHeader32 header;
        header.OffsetInSectionNameStringTable = nameOffset;
        header.Type = SectionType::StrTab;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.Offset;  // TODO: set when writing
        header.SectionSize = static_cast<uint32_t>(shstrtabBuffer.size());
        header.LinkIndex = 0;
        header.Info = 0;
        header.AddressAlignment = 1;
        header.EntrySize = 0;
        
        shstrtab.header = header;
    }
    else if (bits == BitMode::Bits64)
    {
        SectionHeader64 header;
        header.OffsetInSectionNameStringTable = nameOffset;
        header.Type = SectionType::StrTab;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.Offset;  // TODO: set when writing
        header.SectionSize = static_cast<uint32_t>(shstrtabBuffer.size());
        header.LinkIndex = 0;
        header.Info = 0;
        header.AddressAlignment = 1;
        header.EntrySize = 0;

        shstrtab.header = header;
    }
    else throw Exception::InternalError("Unknown bit mode");

    uint16_t shstrtabIndex = static_cast<uint16_t>(sections.size());

    sections.push_back(shstrtab);

    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
    {
        Header32 header;
        header.Bitness = Bitness::BITS32;
        header.Endianness = Endianness::LITTLE;
        header.HeaderVersion = 1;
        header.ABI = 0; // TODO

        header.Type = Type::RELOCATABLE;
        switch (arch)
        {
            case Architecture::x86:
                header.InstructionSet = InstructionSet::X86;
                break;

            case Architecture::ARM:
                header.InstructionSet = InstructionSet::ARM;
                break;

            case Architecture::RISC_V:
                header.InstructionSet = InstructionSet::RISCV;
                break;

            default: throw Exception::InternalError("Unknown architecture");
        }
        header.Version = 1;

        header.ProgramEntryPosition = 0;
        header.ProgramHeaderTablePosition = 0;
        header.SectionHeaderTablePosition = 0x40; // TODO: works, but not optimal

        switch (arch)
        {
            case Architecture::x86:
                header.Flags = 0;
                break;

            case Architecture::ARM:
                header.Flags = 0; // FIXME
                break;

            case Architecture::RISC_V:
                header.Flags = 0; // FIXME
                break;

            default: throw Exception::InternalError("Unknown architecture");
        }

        header.HeaderSize = sizeof(Header32);

        //header.ProgramHeaderTableEntrySize = sizeof(ProgramHeader32); TODO
        header.ProgramHeaderTableEntrySize = 0;
        header.ProgramHeaderTableEntryCount = 0;

        header.SectionHeaderTableEntrySize = sizeof(SectionHeader32);
        header.SectionHeaderTableEntryCount = static_cast<uint16_t>(sections.size());
        header.SectionNamesIndex = shstrtabIndex;

        file->write(reinterpret_cast<const char*>(&header), sizeof(header));
    }
    else if (bits == BitMode::Bits64)
    {
        Header64 header;
        header.Bitness = Bitness::BITS64;
        header.Endianness = Endianness::LITTLE;
        header.HeaderVersion = 1;
        header.ABI = 0; // TODO

        header.Type = Type::RELOCATABLE;
        switch (arch)
        {
            case Architecture::x86:
                header.InstructionSet = InstructionSet::X64;
                break;

            case Architecture::ARM:
                header.InstructionSet = InstructionSet::ARM64;
                break;

            case Architecture::RISC_V:
                header.InstructionSet = InstructionSet::RISCV;
                break;

            default: throw Exception::InternalError("Unknown architecture");
        }
        header.Version = 1;

        header.ProgramEntryPosition = 0;
        header.ProgramHeaderTablePosition = 0;
        header.SectionHeaderTablePosition = 0x40; // TODO: works, but not optimal

        switch (arch)
        {
            case Architecture::x86:
                header.Flags = 0;
                break;

            case Architecture::ARM:
                header.Flags = 0; // FIXME
                break;

            case Architecture::RISC_V:
                header.Flags = 0; // FIXME
                break;

            default: throw Exception::InternalError("Unknown architecture");
        }

        header.HeaderSize = sizeof(Header64);

        //header.ProgramHeaderTableEntrySize = sizeof(ProgramHeader64); TODO
        header.ProgramHeaderTableEntrySize = 0;
        header.ProgramHeaderTableEntryCount = 0;

        header.SectionHeaderTableEntrySize = sizeof(SectionHeader64);
        header.SectionHeaderTableEntryCount = static_cast<uint16_t>(sections.size());
        header.SectionNamesIndex = shstrtabIndex;

        file->write(reinterpret_cast<const char*>(&header), sizeof(header));
    }
    else
    {
        throw Exception::InternalError("Unknown bit mode");
    }
    auto pos = file->tellp();
    std::streamoff padSize = (alignTo - (pos % alignTo)) % alignTo;
    if (padSize)
    {
        std::vector<char> padding(padSize, 0);
        file->write(padding.data(), padSize);
    }

    for (const auto& section : sections)
    {
        if (std::holds_alternative<SectionHeader32>(section.header))
        {
            SectionHeader32 header = std::get<SectionHeader32>(section.header);
            // TODO: set offset
        }
        else if (std::holds_alternative<SectionHeader64>(section.header))
        {
            SectionHeader64 header = std::get<SectionHeader64>(section.header);
            // TODO: set offset

            file->write(reinterpret_cast<const char*>(&header), sizeof(header));
        }
        else throw Exception::InternalError("Unknown section header");
    }
}