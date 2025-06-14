#include "elf.hpp"

#include "relocation.hpp"
#include "symbols.hpp"

#include "../../util/buffer.hpp"
#include "util.hpp"
#include "flags.hpp"

namespace ELF {
    Header createHeader(BitMode bits, Architecture arch, Endianness endianness)
    {
        Header header;

        switch (bits)
        {
            case BitMode::Bits16:
            case BitMode::Bits32:
                header.Bitness = HBitness::Bits32;
                break;
            case BitMode::Bits64:
                header.Bitness = HBitness::Bits64;
                break;
            default:
                header.Bitness = HBitness::None;
                break;
        }

        if (endianness == Endianness::Little)
            header.Endianness = HEndianness::LittleEndian;
        else
            header.Endianness = HEndianness::BigEndian;
        header.HeaderVersion = 1;
        header.ABI = 0;
        header.ABIVersion = 0;

        header.Type = HType::Relocatable;

        switch (arch)
        {
            case Architecture::x86:
                header.Flags = 0;

                if (bits == BitMode::Bits64)
                {
                    header.InstructionSet = HInstructionSet::x64;
                }
                else
                {
                    header.InstructionSet = HInstructionSet::x86;
                }
                break;
            case Architecture::ARM:
                if (bits == BitMode::Bits64)
                {
                    header.Flags = 0;
                    header.InstructionSet = HInstructionSet::Arm64;
                }
                else
                {
                    header.Flags = ARM::Flags32::newABI | ARM::Flags32::vfpFloat;
                    header.InstructionSet = HInstructionSet::Arm;
                }
                break;
            case Architecture::RISC_V:
                header.Flags = RISC_V::Flags::floatABI_double;
                header.InstructionSet = HInstructionSet::riscv;
                break;
            default:
                header.Flags = 0;
                header.InstructionSet = HInstructionSet::None;
                break;
        }

        header.Version = 1;

        header.setProgramEntryPosition(0);
        header.setProgramHeaderTablePosition(0);

        header.setSectionHeaderTablePosition(0);

        header.HeaderSize = sizeof(Header);

        header.ProgramHeaderTableEntrySize = 0;
        header.ProgramHeaderTableEntryCount = 0;
        header.SectionHeaderTableEntrySize = 0;
        header.SectionHeaderTableEntryCount = 0;
        header.SectionNamesIndex = 0;

        return header;
    }

    Data create(BitMode bits, Architecture arch, Endianness endianness, Encoded encoded, Parsed parsed)
    {
        Data data;
        data.header = createHeader(bits, arch, endianness);

        SectionHeader64 dummyHeader64{};
        dummyHeader64.Type = SectionType::Null;  // Oder ein anderer Dummy-Typ, nur nicht symtab/strtab
        dummyHeader64.offsetInSectionNameStringTable = 0;
        dummyHeader64.Flags = 0;
        dummyHeader64.virtualAddress = 0;
        dummyHeader64.fileOffset = 0;
        dummyHeader64.sectionSize = 0;
        dummyHeader64.linkIndex = 0;
        dummyHeader64.info = 0;
        dummyHeader64.addressAlignment = 1;
        dummyHeader64.entrySize = 0;

        ELFSection dummy1;
        dummy1.name = "dummy1";
        dummy1.buffer = {};
        dummy1.header = dummyHeader64;

        ELFSection dummy2;
        dummy2.name = "dummy2";
        dummy2.buffer = {};
        dummy2.header = dummyHeader64;

        ELFSection nullSection;
        nullSection.name = "";
        nullSection.buffer = {};
        
        if(data.header.Bitness == HBitness::Bits64)
        {
            SectionHeader64 nullHeader{};
            nullHeader.Type = SectionType::Null;
            nullSection.header = nullHeader;
        }
        else
        {
            SectionHeader32 nullHeader{};
            nullHeader.Type = SectionType::Null;
            nullSection.header = nullHeader;
        }

        data.sections.push_back(std::move(nullSection));  // Index 0
        data.sections.push_back(std::move(dummy1));       // Index 1
        data.sections.push_back(std::move(dummy2));       // Index 2


        uint32_t symtabIndex = 1;
        uint32_t strtabIndex = 2;

        ELFSection symtab;
        symtab.name = ".symtab";

        std::vector<unsigned char> localSymtabBuffer;
        uint64_t localSymbolCount = 0;
        std::vector<unsigned char> globalSymtabBuffer;
        
        if (data.header.Bitness == HBitness::Bits64)
        {
            SectionHeader64 header;

            header.offsetInSectionNameStringTable = 0;
            header.Type = SectionType::SymTab;
            header.Flags = 0;
            header.virtualAddress = 0;
            header.fileOffset = 0;
            header.sectionSize = 0;
            header.linkIndex = strtabIndex;
            header.info = 0;
            header.addressAlignment = 8;
            header.entrySize = sizeof(Sym64);

            symtab.header = header;

            Sym64 nullSym = {0, 0, 0, 0, 0, 0};
            localSymbolCount++;

            Endian::write(localSymtabBuffer, nullSym.nameOffset, endianness);
            Endian::write(localSymtabBuffer, nullSym.info, endianness);
            Endian::write(localSymtabBuffer, nullSym.other, endianness);
            Endian::write(localSymtabBuffer, nullSym.sectionIndex, endianness);
            Endian::write(localSymtabBuffer, nullSym.value, endianness);
            Endian::write(localSymtabBuffer, nullSym.size, endianness);
        }
        else
        {
            SectionHeader32 header;

            header.offsetInSectionNameStringTable = 0;
            header.Type = SectionType::SymTab;
            header.Flags = 0;
            header.virtualAddress = 0;
            header.fileOffset = 0;
            header.sectionSize = 0;
            header.linkIndex = strtabIndex;
            header.info = 0;
            header.addressAlignment = 4;
            header.entrySize = sizeof(Sym32);

            symtab.header = header;

            Sym32 nullSym = {0, 0, 0, 0, 0, 0};
            localSymbolCount++;
            
            Endian::write(localSymtabBuffer, nullSym.nameOffset, endianness);
            Endian::write(localSymtabBuffer, nullSym.info, endianness);
            Endian::write(localSymtabBuffer, nullSym.other, endianness);
            Endian::write(localSymtabBuffer, nullSym.sectionIndex, endianness);
            Endian::write(localSymtabBuffer, nullSym.value, endianness);
            Endian::write(localSymtabBuffer, nullSym.size, endianness);
        }

        ELFSection strtab;
        strtab.name = ".strtab";
        strtab.buffer.push_back(0);

        for (const auto& externSymbol : parsed.externs)
        {
            uint32_t offsetInStrtab = strtab.buffer.size();
            strtab.buffer.insert(strtab.buffer.end(), externSymbol.begin(), externSymbol.end());
            strtab.buffer.push_back('\0');

            if (data.header.Bitness == HBitness::Bits64)
            {
                Sym64 sym;
                sym.nameOffset = offsetInStrtab;
                sym.info = makeSymbolInfo(SymbolBind::global, SymbolType::notype);
                sym.other = 0;
                sym.sectionIndex = (uint16_t)SectionIndex::UNDEFINED;
                sym.value = 0;
                sym.size = 0;

                Endian::write(globalSymtabBuffer, sym.nameOffset, endianness);
                Endian::write(globalSymtabBuffer, sym.info, endianness);
                Endian::write(globalSymtabBuffer, sym.other, endianness);
                Endian::write(globalSymtabBuffer, sym.sectionIndex, endianness);
                Endian::write(globalSymtabBuffer, sym.value, endianness);
                Endian::write(globalSymtabBuffer, sym.size, endianness);
            }
            else
            {
                Sym32 sym;
                sym.nameOffset = offsetInStrtab;
                sym.info = makeSymbolInfo(SymbolBind::global, SymbolType::notype);;
                sym.other = 0;
                sym.sectionIndex = (uint16_t)SectionIndex::UNDEFINED;
                sym.value = 0;
                sym.size = 0;

                Endian::write(globalSymtabBuffer, sym.nameOffset, endianness);
                Endian::write(globalSymtabBuffer, sym.info, endianness);
                Endian::write(globalSymtabBuffer, sym.other, endianness);
                Endian::write(globalSymtabBuffer, sym.sectionIndex, endianness);
                Endian::write(globalSymtabBuffer, sym.value, endianness);
                Endian::write(globalSymtabBuffer, sym.size, endianness);
            }
        }

        for (const auto& [name, section] : encoded.sections) {
            ELFSection elfsection;
            elfsection.buffer = section.buffer;
            elfsection.name = section.name;

            if (!section.relocations.empty())
            {
                ELFSection relocationSection;
                relocationSection.name = ".rela." + section.name;

                relocationSection.buffer = encodeRelocations(section.relocations, data.header.Bitness);

                if (data.header.Bitness == HBitness::Bits64)
                {
                    SectionHeader64 header;

                    header.offsetInSectionNameStringTable = 0;
                    header.Type = SectionType::Rela;
                    header.Flags = 0;
                    header.virtualAddress = 0;
                    header.fileOffset = 0;
                    header.sectionSize = relocationSection.buffer.size();
                    header.linkIndex = symtabIndex;
                    header.info = (uint64_t)data.sections.size() + 1;
                    header.addressAlignment = 8;
                    header.entrySize = sizeof(Rela64);

                    relocationSection.header = header;
                }
                else
                {
                    SectionHeader32 header;

                    header.offsetInSectionNameStringTable = 0;
                    header.Type = SectionType::Rela;
                    header.Flags = 0;
                    header.virtualAddress = 0;
                    header.fileOffset = 0;
                    header.sectionSize = relocationSection.buffer.size();
                    header.linkIndex = symtabIndex;
                    header.info = (uint32_t)data.sections.size() + 1;
                    header.addressAlignment = 4;
                    header.entrySize = sizeof(Rela32);

                    relocationSection.header = header;
                }

                data.sections.push_back(std::move(relocationSection));
            }

            if (data.header.Bitness == HBitness::Bits64)
            {
                SectionHeader64 header;

                header.offsetInSectionNameStringTable = 0;
                header.Type = getSectionType(elfsection.name);
                header.Flags = getFlags64(elfsection.name);
                header.virtualAddress = 0;
                header.fileOffset = 0;
                header.sectionSize = elfsection.buffer.size();
                header.linkIndex = 0;
                header.info = 0;
                header.addressAlignment = getAlignment64(elfsection.name);
                header.entrySize = 0;

                elfsection.header = header;
            }
            else
            {
                SectionHeader32 header;

                header.offsetInSectionNameStringTable = 0;
                header.Type = getSectionType(elfsection.name);
                header.Flags = getFlags32(elfsection.name);
                header.virtualAddress = 0;
                header.fileOffset = 0;
                header.sectionSize = elfsection.buffer.size();
                header.linkIndex = 0;
                header.info = 0;
                header.addressAlignment = getAlignment32(elfsection.name);
                header.entrySize = 0;

                elfsection.header = header;
            }

            if (!section.labels.empty())
            {
                for (const auto& [name, label] : section.labels)
                {
                    uint32_t nameOffset = strtab.buffer.size();
                    strtab.buffer.insert(strtab.buffer.end(), label.name.begin(), label.name.end());
                    strtab.buffer.push_back('\0');

                    uint16_t sectionIndex = data.sections.size();

                    if (data.header.Bitness == HBitness::Bits64)
                    {
                        Sym64 sym;
                        sym.nameOffset = nameOffset;
                        sym.info = makeSymbolInfo(label.isGlobal ? SymbolBind::global : SymbolBind::local, SymbolType::notype);
                        sym.other = 0;
                        sym.sectionIndex = sectionIndex;
                        sym.value = label.offset;
                        sym.size = 0;

                        if (label.isGlobal)
                        {
                            Endian::write(globalSymtabBuffer, sym.nameOffset, endianness);
                            Endian::write(globalSymtabBuffer, sym.info, endianness);
                            Endian::write(globalSymtabBuffer, sym.other, endianness);
                            Endian::write(globalSymtabBuffer, sym.sectionIndex, endianness);
                            Endian::write(globalSymtabBuffer, sym.value, endianness);
                            Endian::write(globalSymtabBuffer, sym.size, endianness);
                        }
                        else
                        {
                            localSymbolCount++;
                            Endian::write(localSymtabBuffer, sym.nameOffset, endianness);
                            Endian::write(localSymtabBuffer, sym.info, endianness);
                            Endian::write(localSymtabBuffer, sym.other, endianness);
                            Endian::write(localSymtabBuffer, sym.sectionIndex, endianness);
                            Endian::write(localSymtabBuffer, sym.value, endianness);
                            Endian::write(localSymtabBuffer, sym.size, endianness);
                        }
                    }
                    else
                    {
                        Sym32 sym;
                        sym.nameOffset = nameOffset;
                        sym.info = makeSymbolInfo(label.isGlobal ? SymbolBind::global : SymbolBind::local, SymbolType::notype);
                        sym.other = 0;
                        sym.sectionIndex = sectionIndex;
                        sym.value = label.offset;
                        sym.size = 0;

                        if (label.isGlobal)
                        {
                            Endian::write(globalSymtabBuffer, sym.nameOffset, endianness);
                            Endian::write(globalSymtabBuffer, sym.info, endianness);
                            Endian::write(globalSymtabBuffer, sym.other, endianness);
                            Endian::write(globalSymtabBuffer, sym.sectionIndex, endianness);
                            Endian::write(globalSymtabBuffer, sym.value, endianness);
                            Endian::write(globalSymtabBuffer, sym.size, endianness);
                        }
                        else
                        {
                            localSymbolCount++;
                            Endian::write(localSymtabBuffer, sym.nameOffset, endianness);
                            Endian::write(localSymtabBuffer, sym.info, endianness);
                            Endian::write(localSymtabBuffer, sym.other, endianness);
                            Endian::write(localSymtabBuffer, sym.sectionIndex, endianness);
                            Endian::write(localSymtabBuffer, sym.value, endianness);
                            Endian::write(localSymtabBuffer, sym.size, endianness);
                        }
                    }

                    // local variables
                    for (const auto& [lName, localLabel] : label.localLabels) {
                        std::string localName = "@" + name + lName;
                        uint32_t localNameOffset = strtab.buffer.size();
                        strtab.buffer.insert(strtab.buffer.end(), localName.begin(), localName.end());
                        strtab.buffer.push_back('\0');

                        if (data.header.Bitness == HBitness::Bits64) {
                            Sym64 sym;
                            sym.nameOffset = localNameOffset;
                            sym.info = makeSymbolInfo(SymbolBind::local, SymbolType::notype);
                            sym.other = 0;
                            sym.sectionIndex = sectionIndex;
                            sym.value = localLabel.offset;
                            sym.size = 0;

                            localSymbolCount++;
                            Endian::write(localSymtabBuffer, sym.nameOffset, endianness);
                            Endian::write(localSymtabBuffer, sym.info, endianness);
                            Endian::write(localSymtabBuffer, sym.other, endianness);
                            Endian::write(localSymtabBuffer, sym.sectionIndex, endianness);
                            Endian::write(localSymtabBuffer, sym.value, endianness);
                            Endian::write(localSymtabBuffer, sym.size, endianness);
                        } else {
                            Sym32 sym;
                            sym.nameOffset = localNameOffset;
                            sym.info = makeSymbolInfo(SymbolBind::local, SymbolType::notype);
                            sym.other = 0;
                            sym.sectionIndex = sectionIndex;
                            sym.value = localLabel.offset;
                            sym.size = 0;

                            localSymbolCount++;
                            Endian::write(localSymtabBuffer, sym.nameOffset, endianness);
                            Endian::write(localSymtabBuffer, sym.info, endianness);
                            Endian::write(localSymtabBuffer, sym.other, endianness);
                            Endian::write(localSymtabBuffer, sym.sectionIndex, endianness);
                            Endian::write(localSymtabBuffer, sym.value, endianness);
                            Endian::write(localSymtabBuffer, sym.size, endianness);
                        }
                    }
                }
            }

            data.sections.push_back(std::move(elfsection));
        }

        if (data.header.Bitness == HBitness::Bits64)
        {
            SectionHeader64 header;

            header.offsetInSectionNameStringTable = 0;
            header.Type = SectionType::StrTab;
            header.Flags = 0;
            header.virtualAddress = 0;
            header.fileOffset = 0;
            header.sectionSize = strtab.buffer.size();
            header.linkIndex = 0;
            header.info = 0;
            header.addressAlignment = 1;
            header.entrySize = 0;

            strtab.header = header;
        }
        else
        {
            SectionHeader32 header;

            header.offsetInSectionNameStringTable = 0;
            header.Type = SectionType::StrTab;
            header.Flags = 0;
            header.virtualAddress = 0;
            header.fileOffset = 0;
            header.sectionSize = strtab.buffer.size();
            header.linkIndex = 0;
            header.info = 0;
            header.addressAlignment = 1;
            header.entrySize = 0;

            strtab.header = header;
        }

        symtab.buffer = localSymtabBuffer;
        symtab.buffer.insert(symtab.buffer.end(), globalSymtabBuffer.begin(), globalSymtabBuffer.end());

        if (std::holds_alternative<SectionHeader64>(symtab.header))
        {
            SectionHeader64 newHeader = std::get<SectionHeader64>(symtab.header);
            newHeader.info = localSymbolCount;
            newHeader.sectionSize = symtab.buffer.size();
            symtab.header = newHeader;
        }
        else if (std::holds_alternative<SectionHeader32>(symtab.header))
        {
            SectionHeader32 newHeader = std::get<SectionHeader32>(symtab.header);
            newHeader.info = localSymbolCount;
            newHeader.sectionSize = symtab.buffer.size();
            symtab.header = newHeader;
        }

        data.sections[symtabIndex] = std::move(symtab);
        data.sections[strtabIndex] = std::move(strtab);


        ELFSection shstrtab;
        shstrtab.name = ".shstrtab";

        std::unordered_map<std::string, uint32_t> nameOffsets;

        // Für alle Sections (inkl. symtab, strtab, relocations...), Namen hinzufügen
        for (const auto& section : data.sections) {
            nameOffsets[section.name] = shstrtab.buffer.size();
            // Namen als null-terminierte Strings anfügen
            shstrtab.buffer.insert(shstrtab.buffer.end(), section.name.begin(), section.name.end());
            shstrtab.buffer.push_back('\0');
        }

        nameOffsets[shstrtab.name] = shstrtab.buffer.size();
        shstrtab.buffer.insert(shstrtab.buffer.end(), shstrtab.name.begin(), shstrtab.name.end());
        shstrtab.buffer.push_back('\0');

        // Jetzt setze in jedem SectionHeader das offsetInSectionNameStringTable auf den ermittelten Wert:
        for (auto& section : data.sections) {
            uint32_t offset = nameOffsets[section.name];
            std::visit([offset](auto& header){
                header.offsetInSectionNameStringTable = offset;
            }, section.header);
        }

        if (data.header.Bitness == HBitness::Bits64)
        {
            SectionHeader64 header;

            header.offsetInSectionNameStringTable = nameOffsets[shstrtab.name];
            header.Type = SectionType::StrTab;
            header.Flags = 0;
            header.virtualAddress = 0;
            header.fileOffset = 0;
            header.sectionSize = shstrtab.buffer.size();
            header.linkIndex = 0;
            header.info = 0;
            header.addressAlignment = 1;
            header.entrySize = 0;

            shstrtab.header = header;
        }
        else
        {
            SectionHeader32 header;

            header.offsetInSectionNameStringTable = nameOffsets[shstrtab.name];
            header.Type = SectionType::StrTab;
            header.Flags = 0;
            header.virtualAddress = 0;
            header.fileOffset = 0;
            header.sectionSize = shstrtab.buffer.size();
            header.linkIndex = 0;
            header.info = 0;
            header.addressAlignment = 1;
            header.entrySize = 0;

            shstrtab.header = header;
        }

        data.header.SectionNamesIndex = data.sections.size();
        data.sections.push_back(std::move(shstrtab));

        if (data.header.Bitness == HBitness::Bits64)
            data.header.SectionHeaderTableEntrySize = sizeof(SectionHeader64);
        else
            data.header.SectionHeaderTableEntrySize = sizeof(SectionHeader32);
        
        data.header.SectionHeaderTableEntryCount = data.sections.size();

        return data;
    }


    uint64_t writeHeader(std::ofstream& out, const Header& header, Endianness endianness)
    {
        uint64_t offset = 0;

        Endian::write(out, header.Magic, 4, endianness);
        offset += 4;

        // Bitness, Endianness, HeaderVersion, ABI, ABIVersion, Padding[7]
        Endian::write(out, static_cast<uint8_t>(header.Bitness), endianness);
        Endian::write(out, static_cast<uint8_t>(header.Endianness), endianness);
        Endian::write(out, header.HeaderVersion, endianness);
        Endian::write(out, header.ABI, endianness);
        Endian::write(out, header.ABIVersion, endianness);
        Endian::write(out, header._Padding, 7, endianness);
        offset += 1 + 1 + 1 + 1 + 1 + 7;

        // Type, InstructionSet
        Endian::write(out, static_cast<uint16_t>(header.Type), endianness);
        Endian::write(out, static_cast<uint16_t>(header.InstructionSet), endianness);
        offset += 2 + 2;

        // Version (uint32_t)
        Endian::write(out, header.Version, endianness);
        offset += 4;

        if (header.is64Bit())
        {
            Endian::write(out, header.bits64.ProgramEntryPosition, endianness);
            Endian::write(out, header.bits64.ProgramHeaderTablePosition, endianness);
            Endian::write(out, header.bits64.SectionHeaderTablePosition, endianness);
            offset += 8 + 8 + 8;
        }
        else
        {
            Endian::write(out, header.bits32.ProgramEntryPosition, endianness);
            Endian::write(out, header.bits32.ProgramHeaderTablePosition, endianness);
            Endian::write(out, header.bits32.SectionHeaderTablePosition, endianness);
            offset += 4 + 4 + 4;
        }

        // Flags (uint32_t)
        Endian::write(out, header.Flags, endianness);
        offset += 4;

        // HeaderSize, ProgramHeaderTableEntrySize, ProgramHeaderTableEntryCount,
        // SectionHeaderTableEntrySize, SectionHeaderTableEntryCount, SectionNamesIndex (jeweils uint16_t)
        Endian::write(out, header.HeaderSize, endianness);
        Endian::write(out, header.ProgramHeaderTableEntrySize, endianness);
        Endian::write(out, header.ProgramHeaderTableEntryCount, endianness);
        Endian::write(out, header.SectionHeaderTableEntrySize, endianness);
        Endian::write(out, header.SectionHeaderTableEntryCount, endianness);
        Endian::write(out, header.SectionNamesIndex, endianness);
        offset += 2 + 2 + 2 + 2 + 2 + 2;

        return offset;
    }

    void write(std::ofstream& out, Endianness endianness, Data& data)
    {
        uint64_t offset = writeHeader(out, data.header, endianness);

        std::unordered_map<std::string, uint64_t> offsets;

        for (const auto& section : data.sections)
        {
            out.write(reinterpret_cast<const char*>(section.buffer.data()), section.buffer.size());
            offsets[section.name] = offset;
            offset += section.buffer.size();
        }

        data.header.setSectionHeaderTablePosition(offset);

        for (auto& section : data.sections)
        {
            if (std::holds_alternative<SectionHeader64>(section.header))
            {
                SectionHeader64& hdr = std::get<SectionHeader64>(section.header);
                hdr.fileOffset = offsets[section.name];
                
                Endian::write(out, hdr.offsetInSectionNameStringTable, endianness);
                Endian::write(out, static_cast<uint32_t>(hdr.Type), endianness);
                Endian::write(out, hdr.Flags, endianness);
                Endian::write(out, hdr.virtualAddress, endianness);
                Endian::write(out, hdr.fileOffset, endianness);
                Endian::write(out, hdr.sectionSize, endianness);
                Endian::write(out, hdr.linkIndex, endianness);
                Endian::write(out, hdr.info, endianness);
                Endian::write(out, hdr.addressAlignment, endianness);
                Endian::write(out, hdr.entrySize, endianness);
            }
            else
            {
                SectionHeader32& hdr = std::get<SectionHeader32>(section.header);
                hdr.fileOffset = offsets[section.name];
                
                Endian::write(out, hdr.offsetInSectionNameStringTable, endianness);
                Endian::write(out, static_cast<uint32_t>(hdr.Type), endianness);
                Endian::write(out, hdr.Flags, endianness);
                Endian::write(out, hdr.virtualAddress, endianness);
                Endian::write(out, hdr.fileOffset, endianness);
                Endian::write(out, hdr.sectionSize, endianness);
                Endian::write(out, hdr.linkIndex, endianness);
                Endian::write(out, hdr.info, endianness);
                Endian::write(out, hdr.addressAlignment, endianness);
                Endian::write(out, hdr.entrySize, endianness);
            }
        }

        out.seekp(0, std::ios::beg);
        writeHeader(out, data.header, endianness);
    }
};