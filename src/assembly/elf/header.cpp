#include "header.hpp"

#include "relocation.hpp"
#include "symbols.hpp"

#include "../util/buffer.hpp"

namespace ELF {
    Header createHeader(BitMode bits, Architecture arch)
    {
        Header header;

        switch (bits)
        {
            case BitMode::Bits16:
            case BitMode::Bits32:
                header.Bitness = Bitness::Bits32;
                break;
            case BitMode::Bits64:
                header.Bitness = Bitness::Bits64;
                break;
            default:
                header.Bitness = Bitness::None;
                break;
        }

        header.Endianness = Endianness::LittleEndian;
        header.HeaderVersion = 1;
        header.ABI = 0;
        header.ABIVersion = 0;

        header.Type = Type::Relocatable;

        switch (arch)
        {
            case Architecture::x86:
                if (bits == BitMode::Bits64)
                    header.InstructionSet = InstructionSet::x64;
                else
                    header.InstructionSet = InstructionSet::x86;
                break;
            case Architecture::ARM:
                if (bits == BitMode::Bits64)
                    header.InstructionSet = InstructionSet::Arm64;
                else
                    header.InstructionSet = InstructionSet::Arm;
                break;
            case Architecture::RISC_V:
                header.InstructionSet = InstructionSet::riscv;
                break;
            default:
                header.InstructionSet = InstructionSet::None;
                break;
        }

        header.Version = 1;

        header.setProgramEntryPosition(0);
        header.setProgramHeaderTablePosition(0);

        //TODO
        header.setSectionHeaderTablePosition(0);

        header.Flags = 0;
        header.HeaderSize = sizeof(Header);

        //TODO
        header.ProgramHeaderTableEntrySize = 0;
        header.ProgramHeaderTableEntryCount = 0;
        header.SectionHeaderTableEntrySize = 0;
        header.SectionHeaderTableEntryCount = 0;
        header.SectionNamesIndex = 0;

        return header;
    }

    SectionType getSectionType(std::string name)
    {
        return SectionType::ProgBits;
    }

    uint64_t getFlags64(std::string name)
    {
        return 0;
    }

    uint32_t getFlags32(std::string name)
    {
        return 0;
    }

    uint64_t getAlignment64(std::string name)
    {
        return 0;
    }

    uint32_t getAlignment32(std::string name)
    {
        return 0;
    }

    Data createELF(BitMode bits, Architecture arch, Encoded encoded, Parsed parsed)
    {
        Data data;
        data.header = createHeader(bits, arch);

        //TODO: SHT_NULL

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
        SectionHeader64 nullHeader{};
        nullHeader.Type = SectionType::Null;
        nullSection.header = nullHeader;

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
        
        if (data.header.Bitness == Bitness::Bits64)
        {
            SectionHeader64 header;

            header.offsetInSectionNameStringTable = 0; //TODO
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
            writeToBuffer(symtab.buffer, nullSym);
        }
        else
        {
            SectionHeader32 header;

            header.offsetInSectionNameStringTable = 0; //TODO
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
            writeToBuffer(symtab.buffer, nullSym);
        }

        ELFSection strtab;
        strtab.name = ".strtab";
        strtab.buffer.push_back(0);

        for (const auto& [name, section] : encoded.sections) {
            ELFSection elfsection;
            elfsection.buffer = section.buffer;
            elfsection.name = section.name;

            //TODO: labels

            if (!section.relocations.empty())
            {
                ELFSection relocationSection;
                relocationSection.name = ".rela." + section.name;

                relocationSection.buffer = encodeRelocations(section.relocations, data.header.Bitness);

                if (data.header.Bitness == Bitness::Bits64)
                {
                    SectionHeader64 header;

                    header.offsetInSectionNameStringTable = 0; //TODO
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

                    header.offsetInSectionNameStringTable = 0; //TODO
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

            if (data.header.Bitness == Bitness::Bits64)
            {
                SectionHeader64 header;

                header.offsetInSectionNameStringTable = 0; //TODO
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

                header.offsetInSectionNameStringTable = 0; //TODO
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

            data.sections.push_back(std::move(elfsection));
        }

        if (data.header.Bitness == Bitness::Bits64)
        {
            SectionHeader64 header;

            header.offsetInSectionNameStringTable = 0; //TODO
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

            header.offsetInSectionNameStringTable = 0; //TODO
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

        std::visit([&](auto& hdr) {
            hdr.info = static_cast<decltype(hdr.info)>(localSymbolCount);
            hdr.sectionSize = static_cast<decltype(hdr.sectionSize)>(symtab.buffer.size());
        }, symtab.header);

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
            shstrtab.buffer.push_back(0);
        }

        nameOffsets[shstrtab.name] = shstrtab.buffer.size();
        shstrtab.buffer.insert(shstrtab.buffer.end(), shstrtab.name.begin(), shstrtab.name.end());
        shstrtab.buffer.push_back(0);

        // Jetzt setze in jedem SectionHeader das offsetInSectionNameStringTable auf den ermittelten Wert:
        for (auto& section : data.sections) {
            uint32_t offset = nameOffsets[section.name];
            std::visit([offset](auto& header){
                header.offsetInSectionNameStringTable = offset;
            }, section.header);
        }

        if (data.header.Bitness == Bitness::Bits64)
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
            header.Type = SectionType::SymTabShndx;
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

        if (data.header.Bitness == Bitness::Bits64) {
            data.header.SectionHeaderTableEntrySize = sizeof(SectionHeader64);
        } else {
            data.header.SectionHeaderTableEntrySize = sizeof(SectionHeader32);
        }
        data.header.SectionHeaderTableEntryCount = data.sections.size();


        for (const auto& section : data.sections)
        {
            std::cout << "Section: " << section.name << std::endl;
            if (std::holds_alternative<SectionHeader32>(section.header))
            {
                const SectionHeader32& hdr32 = std::get<SectionHeader32>(section.header);
                std::cout << "\tOffset in shstrtab: " << hdr32.offsetInSectionNameStringTable << std::endl;

            }
            else if (std::holds_alternative<SectionHeader64>(section.header))
            {
                const SectionHeader64& hdr64 = std::get<SectionHeader64>(section.header);
                std::cout << "\tOffset in shstrtab: " << hdr64.offsetInSectionNameStringTable << std::endl;

            }
        }

        return data;
    }
};