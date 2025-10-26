#include "ELFWriter.hpp"

#include <cstring>

ELF::Writer::Writer(const Context &_context, Architecture _arch, BitMode _bits, Format _format, std::ostream *_file, const Parser::Parser *_parser, const Encoder::Encoder *_encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{
}

void ELF::Writer::Write()
{
    //TODO: extern labels
    constexpr std::streamoff alignTo = 0x10;
    constexpr uint64_t alignment = static_cast<uint64_t>(alignTo);

    std::vector<Encoder::Section> eSections = encoder->getSections();
    const std::vector<Encoder::Encoder::Symbol>& symbols = encoder->getSymbols();
    const std::vector<Encoder::Relocation>& relocations = encoder->getRelocations();

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
    else throw Exception::InternalError("Unknown bit mode", -1, -1);

    sections.push_back(std::move(nullSection));

    std::vector<uint8_t> shstrtabBuffer;
    shstrtabBuffer.push_back(0);

    std::vector<uint8_t> symtabBuffer;
    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
    {
        Symbol::Entry32 entry;
        entry.OffsetInNameStringTable = 0;
        entry.Value = 0;
        entry.Size = 0;
        entry.Info = 0;
        entry.Other = 0;
        entry.IndexInSectionHeaderTable = 0;
        
        localSymbols.push_back(std::move(entry));
    }
    else if (bits == BitMode::Bits64)
    {
        Symbol::Entry64 entry;
        entry.OffsetInNameStringTable = 0;
        entry.Info = 0;
        entry.Other = 0;
        entry.IndexInSectionHeaderTable = 0;
        entry.Value = 0;
        entry.Size = 0;
        
        localSymbols.push_back(std::move(entry));
    }
    else throw Exception::InternalError("Unknown bit mode", -1, -1);

    std::vector<uint8_t> strtabBuffer;
    strtabBuffer.push_back(0);


    uint32_t filenameOffset = static_cast<uint32_t>(strtabBuffer.size());
    strtabBuffer.insert(strtabBuffer.end(), context.filename.begin(), context.filename.end());
    strtabBuffer.push_back(0);

    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
    {
        Symbol::Entry32 entry;
        entry.OffsetInNameStringTable = filenameOffset;
        entry.Value = 0;
        entry.Size = 0;
        entry.Info = Symbol::SetInfo(Symbol::Bind::LOCAL, Symbol::Type::FILE);
        entry.Other = 0;
        entry.IndexInSectionHeaderTable = Symbol::XINDEX;
        
        localSymbols.push_back(std::move(entry));
    }
    else if (bits == BitMode::Bits64)
    {
        Symbol::Entry64 entry;
        entry.OffsetInNameStringTable = filenameOffset;
        entry.Info = Symbol::SetInfo(Symbol::Bind::LOCAL, Symbol::Type::FILE);
        entry.Other = 0;
        entry.IndexInSectionHeaderTable = Symbol::XINDEX;
        entry.Value = 0;
        entry.Size = 0;
        
        localSymbols.push_back(std::move(entry));
    }
    else throw Exception::InternalError("Unknown bit mode", -1, -1);


    std::unordered_map<std::string, uint16_t> sectionIndexes;
    std::unordered_map<std::string, uint64_t> sectionSymbolIndex;

    for (size_t i = 0; i < eSections.size(); i++)
    {
        Encoder::Section& section = eSections[i];
        Section s;
        s.buffer = &section.buffer;
        s.name = section.name;

        uint32_t nameOffset = static_cast<uint32_t>(shstrtabBuffer.size());
        shstrtabBuffer.insert(shstrtabBuffer.end(), section.name.begin(), section.name.end());
        shstrtabBuffer.push_back(0);

        if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
        {
            SectionHeader32 header;
            header.OffsetInSectionNameStringTable = nameOffset;
            header.Type = getSectionType(section.name);
            header.Flags = static_cast<uint32_t>(getSectionFlags(section.name));
            header.VirtualAddress = 0;
            if (section.isInitialized)
                header.SectionSize = static_cast<uint32_t>(section.buffer.size());
            else
                header.SectionSize = static_cast<uint32_t>(section.reservedSize);
            header.LinkIndex = 0;                   // TODO
            header.Info = 0;                        // TODO
            header.AddressAlignment = section.align;
            header.EntrySize = 0;
            
            s.header = header;


            Symbol::Entry32 entry;
            entry.OffsetInNameStringTable = 0;
            entry.Value = 0;
            entry.Size = 0;
            entry.Info = Symbol::SetInfo(Symbol::Bind::LOCAL, Symbol::Type::SECTION);
            entry.Other = 0;
            entry.IndexInSectionHeaderTable = i + 1; // TODO: ugly
            
            sectionSymbolIndex[section.name] = static_cast<uint64_t>(localSymbols.size());
            localSymbols.push_back(std::move(entry));
        }
        else if (bits == BitMode::Bits64)
        {
            SectionHeader64 header;
            header.OffsetInSectionNameStringTable = nameOffset;
            header.Type = getSectionType(section.name);
            header.Flags = getSectionFlags(section.name);
            header.VirtualAddress = 0;
            if (section.isInitialized)
                header.SectionSize = static_cast<uint64_t>(section.buffer.size());
            else
                header.SectionSize = static_cast<uint64_t>(section.reservedSize);
            header.LinkIndex = 0;                   // TODO
            header.Info = 0;                        // TODO
            header.AddressAlignment = section.align;
            header.EntrySize = 0;

            s.header = header;


            Symbol::Entry64 entry;
            entry.OffsetInNameStringTable = 0;
            entry.Info = Symbol::SetInfo(Symbol::Bind::LOCAL, Symbol::Type::SECTION);
            entry.Other = 0;
            entry.IndexInSectionHeaderTable = i + 1; // TODO: ugly
            entry.Value = 0;
            entry.Size = 0;
            
            sectionSymbolIndex[section.name] = static_cast<uint64_t>(localSymbols.size());
            localSymbols.push_back(std::move(entry));
        }
        else throw Exception::InternalError("Unknown bit mode", -1, -1);

        sectionIndexes[section.name] = static_cast<uint16_t>(sections.size());
        sections.push_back(std::move(s));
    }

    // Symbols
    std::unordered_map<std::string, uint32_t> labelNameOffsets;

    for (const auto& symbol : symbols)
    {
        uint32_t offset = static_cast<uint32_t>(strtabBuffer.size());

        if (std::holds_alternative<Encoder::Label*>(symbol))
        {
            const Encoder::Label* label = std::get<Encoder::Label*>(symbol);
            strtabBuffer.insert(strtabBuffer.end(), label->name.begin(), label->name.end());
            strtabBuffer.push_back(0);
            labelNameOffsets[label->name] = offset;
        }
        else if (std::holds_alternative<Encoder::Constant*>(symbol))
        {
            const Encoder::Constant* constant = std::get<Encoder::Constant*>(symbol);
            strtabBuffer.insert(strtabBuffer.end(), constant->name.begin(), constant->name.end());
            strtabBuffer.push_back(0);
            labelNameOffsets[constant->name] = offset;
        }
    }

    for (const auto& symbol : symbols)
    {
        if (std::holds_alternative<Encoder::Label*>(symbol))
        {
            const Encoder::Label* label = std::get<Encoder::Label*>(symbol);

            auto it = labelNameOffsets.find(label->name);
            if (it == labelNameOffsets.end()) throw Exception::InternalError("Couldn't find offset for label in .strtab", -1, -1);
            uint32_t nameOffset = it->second;

            if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
            {
                Symbol::Entry32 entry;
                entry.OffsetInNameStringTable = nameOffset;
                entry.Value = static_cast<uint32_t>(label->offset);
                entry.Size = 0;
                entry.Info = Symbol::SetInfo(label->isGlobal ? Symbol::Bind::GLOBAL : Symbol::Bind::LOCAL, Symbol::Type::NONE);
                entry.Other = 0;
                auto it = sectionIndexes.find(label->section);
                if (it == sectionIndexes.end()) throw Exception::InternalError("Unknown section for label", -1, -1);
                entry.IndexInSectionHeaderTable = it->second;
                
                if (label->isGlobal) globalSymbols.push_back(std::move(entry));
                else localSymbols.push_back(std::move(entry));
            }
            else if (bits == BitMode::Bits64)
            {
                Symbol::Entry64 entry;
                entry.OffsetInNameStringTable = nameOffset;
                entry.Info = Symbol::SetInfo(label->isGlobal ? Symbol::Bind::GLOBAL : Symbol::Bind::LOCAL, Symbol::Type::NONE);
                entry.Other = 0;
                auto it = sectionIndexes.find(label->section);
                if (it == sectionIndexes.end()) throw Exception::InternalError("Unknown section for label", -1, -1);
                entry.IndexInSectionHeaderTable = it->second;
                entry.Value = label->offset;
                entry.Size = 0;
                
                if (label->isGlobal) globalSymbols.push_back(std::move(entry));
                else localSymbols.push_back(std::move(entry));
            }
            else throw Exception::InternalError("Unknown bit mode", -1, -1);
        }
        else if (std::holds_alternative<Encoder::Constant*>(symbol))
        {
            const Encoder::Constant* constant = std::get<Encoder::Constant*>(symbol);
            
            auto it = labelNameOffsets.find(constant->name);
            if (it == labelNameOffsets.end()) throw Exception::InternalError("Couldn't find offset for constant in .strtab", -1, -1);
            uint32_t nameOffset = it->second;

            if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
            {
                Symbol::Entry32 entry;
                entry.OffsetInNameStringTable = nameOffset;
                entry.Value = static_cast<uint32_t>(constant->useOffset ? constant->off : constant->value); // TODO: overflow
                entry.Size = 0;
                entry.Info = Symbol::SetInfo(constant->isGlobal ? Symbol::Bind::GLOBAL : Symbol::Bind::LOCAL, Symbol::Type::NONE);
                entry.Other = 0;
                if (constant->useOffset)
                {
                    auto it = sectionIndexes.find(constant->usedSection);
                    if (it == sectionIndexes.end()) throw Exception::InternalError("Unknown section for label", -1, -1);
                    entry.IndexInSectionHeaderTable = it->second;
                }
                else entry.IndexInSectionHeaderTable = Symbol::XINDEX;
                
                if (constant->isGlobal) globalSymbols.push_back(std::move(entry));
                else localSymbols.push_back(std::move(entry));
            }
            else if (bits == BitMode::Bits64)
            {
                Symbol::Entry64 entry;
                entry.OffsetInNameStringTable = nameOffset;
                entry.Info = Symbol::SetInfo(constant->isGlobal ? Symbol::Bind::GLOBAL : Symbol::Bind::LOCAL, Symbol::Type::NONE);
                entry.Other = 0;
                if (constant->useOffset)
                {
                    auto it = sectionIndexes.find(constant->usedSection);
                    if (it == sectionIndexes.end()) throw Exception::InternalError("Unknown section for label", -1, -1);
                    entry.IndexInSectionHeaderTable = it->second;
                }
                else entry.IndexInSectionHeaderTable = Symbol::XINDEX;
                entry.Value = constant->useOffset ? constant->off : constant->value;
                entry.Size = 0;
                
                if (constant->isGlobal) globalSymbols.push_back(std::move(entry));
                else localSymbols.push_back(std::move(entry));
            }
            else throw Exception::InternalError("Unknown bit mode", -1, -1);
        }
    }

    // SHSTRTAB
    Section shstrtab;
    shstrtab.buffer = &shstrtabBuffer;
    shstrtab.name = ".shstrtab";
    std::string shstrtabName = ".shstrtab";

    uint32_t shstrtabNameOffset = static_cast<uint32_t>(shstrtabBuffer.size());
    shstrtabBuffer.insert(shstrtabBuffer.end(), shstrtabName.begin(), shstrtabName.end());
    shstrtabBuffer.push_back(0);

    uint16_t shstrtabIndex = static_cast<uint16_t>(sections.size());

    // SYMTAB
    Section symtab;
    symtab.buffer = &symtabBuffer;
    symtab.name = ".symtab";
    std::string symtabName = ".symtab";
    uint32_t symtabIndex = static_cast<uint32_t>(sections.size()) + 1; // TODO: ugly way

    uint32_t symtabNameOffset = static_cast<uint32_t>(shstrtabBuffer.size());
    shstrtabBuffer.insert(shstrtabBuffer.end(), symtabName.begin(), symtabName.end());
    shstrtabBuffer.push_back(0);

    for (const SymbolEntry& symbol : localSymbols)
    {
        if (std::holds_alternative<Symbol::Entry32>(symbol))
        {
            const Symbol::Entry32& entry = std::get<Symbol::Entry32>(symbol);
            symtabBuffer.resize(symtabBuffer.size() + sizeof(Symbol::Entry32));
            std::memcpy(symtabBuffer.data() + symtabBuffer.size() - sizeof(Symbol::Entry32), &entry, sizeof(Symbol::Entry32));
        }
        else if (std::holds_alternative<Symbol::Entry64>(symbol))
        {
            const Symbol::Entry64& entry = std::get<Symbol::Entry64>(symbol);
            symtabBuffer.resize(symtabBuffer.size() + sizeof(Symbol::Entry64));
            std::memcpy(symtabBuffer.data() + symtabBuffer.size() - sizeof(Symbol::Entry64), &entry, sizeof(Symbol::Entry64));
        }
        else throw Exception::InternalError("Unknown symbol entry", -1, -1);
    }

    for (const SymbolEntry& symbol : globalSymbols)
    {
        if (std::holds_alternative<Symbol::Entry32>(symbol))
        {
            const Symbol::Entry32& entry = std::get<Symbol::Entry32>(symbol);
            symtabBuffer.resize(symtabBuffer.size() + sizeof(Symbol::Entry32));
            std::memcpy(symtabBuffer.data() + symtabBuffer.size() - sizeof(Symbol::Entry32), &entry, sizeof(Symbol::Entry32));
        }
        else if (std::holds_alternative<Symbol::Entry64>(symbol))
        {
            const Symbol::Entry64& entry = std::get<Symbol::Entry64>(symbol);
            symtabBuffer.resize(symtabBuffer.size() + sizeof(Symbol::Entry64));
            std::memcpy(symtabBuffer.data() + symtabBuffer.size() - sizeof(Symbol::Entry64), &entry, sizeof(Symbol::Entry64));
        }
        else throw Exception::InternalError("Unknown symbol entry", -1, -1);
    }

    for (const SymbolEntry& symbol : weakSymbols)
    {
        if (std::holds_alternative<Symbol::Entry32>(symbol))
        {
            const Symbol::Entry32& entry = std::get<Symbol::Entry32>(symbol);
            symtabBuffer.resize(symtabBuffer.size() + sizeof(Symbol::Entry32));
            std::memcpy(symtabBuffer.data() + symtabBuffer.size() - sizeof(Symbol::Entry32), &entry, sizeof(Symbol::Entry32));
        }
        else if (std::holds_alternative<Symbol::Entry64>(symbol))
        {
            const Symbol::Entry64& entry = std::get<Symbol::Entry64>(symbol);
            symtabBuffer.resize(symtabBuffer.size() + sizeof(Symbol::Entry64));
            std::memcpy(symtabBuffer.data() + symtabBuffer.size() - sizeof(Symbol::Entry64), &entry, sizeof(Symbol::Entry64));
        }
        else throw Exception::InternalError("Unknown symbol entry", -1, -1);
    }

    // STRTAB
    Section strtab;
    strtab.buffer = &strtabBuffer;
    strtab.name = ".strtab";
    std::string strtabName = ".strtab";

    uint32_t strtabNameOffset = static_cast<uint32_t>(shstrtabBuffer.size());
    shstrtabBuffer.insert(shstrtabBuffer.end(), strtabName.begin(), strtabName.end());
    shstrtabBuffer.push_back(0);

    // Relocations
    for (const Encoder::Relocation& relocation : relocations)
    {
        auto it = sectionIndexes.find(relocation.section);
        if (it == sectionIndexes.end()) throw Exception::InternalError("Section not found", -1, -1);
        const uint16_t sectionIndex = it->second;
        Section& section = sections[sectionIndex];

        if (!section.hasRelocations) section.hasRelocations = true;
        if (!section.hasAddend && (bits == BitMode::Bits64 || !relocation.addendInCode)) section.hasAddend = true;

        section.relocations.push_back(relocation);
    }

    for (Section& section : sections)
    {
        if (!section.hasRelocations || section.nullSection) continue;
        RelocationSection relocSection;

        auto getType32 = [](Encoder::RelocationType type, Encoder::RelocationSize size) -> uint8_t
        {
            switch (type)
            {
                case Encoder::RelocationType::Absolute:
                    switch (size)
                    {
                        case Encoder::RelocationSize::Bit8: return RelocationType32::R386_ABS8;
                        case Encoder::RelocationSize::Bit16: return RelocationType32::R386_ABS16;
                        case Encoder::RelocationSize::Bit32: return RelocationType32::R386_ABS32;
                        case Encoder::RelocationSize::Bit64: return RelocationType32::R386_ABS32; // FIXME: not very nice way of doing it
                        default: throw Exception::InternalError("Unknown relocation size", -1, -1);
                    }
                default: throw Exception::InternalError("Unknown relocation type", -1, -1);
            }
            return RelocationType32::R386_None;
        };

        auto getType64 = [](Encoder::RelocationType type, Encoder::RelocationSize size) -> uint32_t
        {
            switch (type)
            {
                case Encoder::RelocationType::Absolute:
                    switch (size)
                    {
                        case Encoder::RelocationSize::Bit8: return RelocationType64::RX64_ABS8;
                        case Encoder::RelocationSize::Bit16: return RelocationType64::RX64_ABS16;
                        case Encoder::RelocationSize::Bit32: return RelocationType64::RX64_ABS32;
                        case Encoder::RelocationSize::Bit64: return RelocationType64::RX64_ABS64;
                        default: throw Exception::InternalError("Unknown relocation size", -1, -1);
                    }
                default: throw Exception::InternalError("Unknown relocation type", -1, -1);
            }
            return RelocationType64::RX64_None;
        };
        
        if (section.hasAddend)
        {
            relocSection.name = ".rela" + section.name;
            for (const Encoder::Relocation& relocation : section.relocations)
            {
                auto it = sectionSymbolIndex.find(relocation.usedSection);
                if (it == sectionSymbolIndex.end()) throw Exception::InternalError("Couldn't find index in .symtab", -1, -1);
                const uint64_t symbolIndex = it->second;

                // TODO: check if actual useful
                // If the relocation has an addend, this code removes it from the code itself
                switch (relocation.size)
                {
                    case Encoder::RelocationSize::Bit8:
                    {
                        std::memset(section.buffer->data() + relocation.offsetInSection, 0, sizeof(uint8_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit16:
                    {
                        std::memset(section.buffer->data() + relocation.offsetInSection, 0, sizeof(uint16_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit24:
                    {
                        std::memset(section.buffer->data() + relocation.offsetInSection, 0, 3);
                        break;
                    }
                    case Encoder::RelocationSize::Bit32:
                    {
                        std::memset(section.buffer->data() + relocation.offsetInSection, 0, sizeof(uint32_t));
                        break;
                    }
                    case Encoder::RelocationSize::Bit64:
                    {
                        std::memset(section.buffer->data() + relocation.offsetInSection, 0, sizeof(uint64_t));
                        break;
                    }
                }
                
                if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
                {
                    RelaEntry32 entry;

                    entry.offset = static_cast<uint32_t>(relocation.offsetInSection); //TODO: handle overflow
                    entry.addend = static_cast<int32_t>(relocation.addend); //TODO: handle overflow

                    uint8_t type = getType32(relocation.type, relocation.size);
                    // TODO: handle overflows with symbol
                    entry.info = SetRelocationInfo32(static_cast<uint32_t>(symbolIndex), type);

                    relocSection.buffer.resize(relocSection.buffer.size() + sizeof(RelaEntry32));
                    std::memcpy(relocSection.buffer.data() + relocSection.buffer.size() - sizeof(RelaEntry32), &entry, sizeof(RelaEntry32));
                }
                else if (bits == BitMode::Bits64)
                {
                    RelaEntry64 entry;

                    entry.offset = relocation.offsetInSection;
                    entry.addend = relocation.addend;

                    uint32_t type = getType64(relocation.type, relocation.size);
                    // TODO: handle overflows with symbol
                    entry.info = SetRelocationInfo64(static_cast<uint32_t>(symbolIndex), type);

                    relocSection.buffer.resize(relocSection.buffer.size() + sizeof(RelaEntry64));
                    std::memcpy(relocSection.buffer.data() + relocSection.buffer.size() - sizeof(RelaEntry64), &entry, sizeof(RelaEntry64));
                }
                else throw Exception::InternalError("Unknown bit mode", -1, -1);
            }
        }
        else
        {
            relocSection.name = ".rel" + section.name;
            for (const Encoder::Relocation& relocation : section.relocations)
            {
                auto it = sectionSymbolIndex.find(relocation.usedSection);
                if (it == sectionSymbolIndex.end()) throw Exception::InternalError("Couldn't find index in .symtab", -1, -1);
                const uint64_t symbolIndex = it->second;
                
                if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
                {
                    RelEntry32 entry;
                    entry.offset = static_cast<uint32_t>(relocation.offsetInSection); //TODO: handle overflow

                    uint8_t type = getType32(relocation.type, relocation.size);
                    // TODO: handle overflows with symbol
                    entry.info = SetRelocationInfo32(static_cast<uint32_t>(symbolIndex), type);

                    relocSection.buffer.resize(relocSection.buffer.size() + sizeof(RelEntry32));
                    std::memcpy(relocSection.buffer.data() + relocSection.buffer.size() - sizeof(RelEntry32), &entry, sizeof(RelEntry32));
                }
                else if (bits == BitMode::Bits64)
                {
                    RelEntry64 entry;
                    entry.offset = relocation.offsetInSection;

                    uint32_t type = getType64(relocation.type, relocation.size);
                    // TODO: handle overflows with symbol
                    entry.info = SetRelocationInfo64(static_cast<uint32_t>(symbolIndex), type);

                    relocSection.buffer.resize(relocSection.buffer.size() + sizeof(RelEntry64));
                    std::memcpy(relocSection.buffer.data() + relocSection.buffer.size() - sizeof(RelEntry64), &entry, sizeof(RelEntry64));
                }
                else throw Exception::InternalError("Unknown bit mode", -1, -1);
            }
        }

        uint32_t nameOffset = static_cast<uint32_t>(shstrtabBuffer.size());
        shstrtabBuffer.insert(shstrtabBuffer.end(), relocSection.name.begin(), relocSection.name.end());
        shstrtabBuffer.push_back(0);

        auto it = sectionIndexes.find(section.name);
        if (it == sectionIndexes.end()) throw Exception::InternalError("Couldn't find section index", -1, -1);
        const uint32_t sectionIndex = it->second;

        if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
        {
            SectionHeader32 header;
            header.OffsetInSectionNameStringTable = nameOffset;
            header.Type = section.hasAddend ? SectionType::Rela : SectionType::Rel;
            header.Flags = 0;
            header.VirtualAddress = 0;
            header.SectionSize = static_cast<uint32_t>(relocSection.buffer.size()); // TODO: overflows
            header.LinkIndex = symtabIndex;
            header.Info = sectionIndex;
            header.AddressAlignment = 4;
            header.EntrySize = section.hasAddend ? sizeof(RelaEntry32) : sizeof(RelEntry32);
            
            relocSection.header = header;
        }
        else if (bits == BitMode::Bits64)
        {
            SectionHeader64 header;
            header.OffsetInSectionNameStringTable = nameOffset;
            header.Type = section.hasAddend ? SectionType::Rela : SectionType::Rel;
            header.Flags = 0;
            header.VirtualAddress = 0;
            header.SectionSize = static_cast<uint64_t>(relocSection.buffer.size());
            header.LinkIndex = symtabIndex;
            header.Info = sectionIndex;
            header.AddressAlignment = 8;
            header.EntrySize = section.hasAddend ? sizeof(RelaEntry64) : sizeof(RelEntry64);

            relocSection.header = header;
        }
        else throw Exception::InternalError("Unknown bit mode", -1, -1);

        relocationSections.push_back(std::move(relocSection));
    }

    // back
    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
    {
        SectionHeader32 header;
        header.OffsetInSectionNameStringTable = shstrtabNameOffset;
        header.Type = SectionType::StrTab;
        header.Flags = 0;
        header.VirtualAddress = 0;
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
        header.OffsetInSectionNameStringTable = shstrtabNameOffset;
        header.Type = SectionType::StrTab;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.SectionSize = static_cast<uint32_t>(shstrtabBuffer.size());
        header.LinkIndex = 0;
        header.Info = 0;
        header.AddressAlignment = 1;
        header.EntrySize = 0;

        shstrtab.header = header;
    }
    else throw Exception::InternalError("Unknown bit mode", -1, -1);
    sections.push_back(std::move(shstrtab));

    const uint32_t strtabIndex = static_cast<uint32_t>(sections.size()) + 1; // TODO: ugly

    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
    {
        SectionHeader32 header;
        header.OffsetInSectionNameStringTable = symtabNameOffset;
        header.Type = SectionType::SymTab;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.SectionSize = static_cast<uint32_t>(symtabBuffer.size());
        header.LinkIndex = strtabIndex;
        header.Info = static_cast<uint32_t>(localSymbols.size());
        header.AddressAlignment = 4;
        header.EntrySize = sizeof(Symbol::Entry32);
        
        symtab.header = header;
    }
    else if (bits == BitMode::Bits64)
    {
        SectionHeader64 header;
        header.OffsetInSectionNameStringTable = symtabNameOffset;
        header.Type = SectionType::SymTab;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.SectionSize = static_cast<uint32_t>(symtabBuffer.size());
        header.LinkIndex = strtabIndex;
        header.Info = static_cast<uint32_t>(localSymbols.size());
        header.AddressAlignment = 8;
        header.EntrySize = sizeof(Symbol::Entry64);

        symtab.header = header;
    }
    else throw Exception::InternalError("Unknown bit mode", -1, -1);
    sections.push_back(std::move(symtab));

    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
    {
        SectionHeader32 header;
        header.OffsetInSectionNameStringTable = strtabNameOffset;
        header.Type = SectionType::StrTab;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.SectionSize = static_cast<uint32_t>(strtabBuffer.size());
        header.LinkIndex = 0;
        header.Info = 0;
        header.AddressAlignment = 1;
        header.EntrySize = 0;
        
        strtab.header = header;
    }
    else if (bits == BitMode::Bits64)
    {
        SectionHeader64 header;
        header.OffsetInSectionNameStringTable = strtabNameOffset;
        header.Type = SectionType::StrTab;
        header.Flags = 0;
        header.VirtualAddress = 0;
        header.SectionSize = static_cast<uint32_t>(strtabBuffer.size());
        header.LinkIndex = 0;
        header.Info = 0;
        header.AddressAlignment = 1;
        header.EntrySize = 0;

        strtab.header = header;
    }
    else throw Exception::InternalError("Unknown bit mode", -1, -1);
    sections.push_back(std::move(strtab));

    // Relocations
    for (RelocationSection& relocationSection : relocationSections)
    {
        // TODO: ugly way to do this
        Section s;
        s.buffer = &relocationSection.buffer;
        s.name = relocationSection.name;
        s.header = relocationSection.header;
        sections.push_back(std::move(s));
    }

    // ELF Header
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

            default: throw Exception::InternalError("Unknown architecture", -1, -1);
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

            default: throw Exception::InternalError("Unknown architecture", -1, -1);
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

            default: throw Exception::InternalError("Unknown architecture", -1, -1);
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

            default: throw Exception::InternalError("Unknown architecture", -1, -1);
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
    else throw Exception::InternalError("Unknown bit mode", -1, -1);
    std::streampos pos = file->tellp();
    std::streamoff padSize = (alignTo - (pos % alignTo)) % alignTo;
    if (padSize)
    {
        std::vector<char> padding(padSize, 0);
        file->write(padding.data(), padSize);
    }

    uint64_t offset = 0x40; // TODO: ugly way
    if (bits == BitMode::Bits16 || bits == BitMode::Bits32)
        offset += static_cast<uint64_t>(sections.size()) * sizeof(SectionHeader32);
    else if (bits == BitMode::Bits64)
        offset += static_cast<uint64_t>(sections.size()) * sizeof(SectionHeader64);
    else throw Exception::InternalError("Unknown bit mode", -1, -1);

    offset = (offset + alignment - 1) / alignment * alignment;

    for (const Section& section : sections)
    {
        if (std::holds_alternative<SectionHeader32>(section.header))
        {
            SectionHeader32 header = std::get<SectionHeader32>(section.header);
            if (!section.nullSection) header.Offset = static_cast<uint64_t>(offset);

            file->write(reinterpret_cast<const char*>(&header), sizeof(header));
        }
        else if (std::holds_alternative<SectionHeader64>(section.header))
        {
            SectionHeader64 header = std::get<SectionHeader64>(section.header);
            if (!section.nullSection) header.Offset = offset;

            file->write(reinterpret_cast<const char*>(&header), sizeof(header));
        }
        else throw Exception::InternalError("Unknown section header", -1, -1);

        if (section.writeBuffer) offset += section.buffer->size();

        offset = (offset + alignment - 1) / alignment * alignment;
    }

    for (const Section& section : sections)
    {
        if (section.writeBuffer) file->write(reinterpret_cast<const char*>(section.buffer->data()), section.buffer->size());

        // align to 0x10
        std::streampos pos = file->tellp();
        std::streamoff padSize = (alignTo - (pos % alignTo)) % alignTo;
        if (padSize)
        {
            std::vector<char> padding(padSize, 0);
            file->write(padding.data(), padSize);
        }
    }
}

// TODO: check why .bss doesn't get written, it's right that way, but it wasn't implemented yet
