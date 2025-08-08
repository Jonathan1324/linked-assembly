#include "ELFWriter.hpp"

ELF::Writer::Writer(const Context &_context, Architecture _arch, BitMode _bits, Format _format, std::ostream *_file, const Parser::Parser *_parser, const Encoder::Encoder *_encoder)
    : ::Output::Writer::Writer(_context, _arch, _bits, _format, _file, _parser, _encoder)
{
}

void ELF::Writer::Write()
{
    

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
        header.SectionHeaderTablePosition = 0; // FIXME

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

        header.ProgramHeaderTableEntrySize; // TODO
        header.ProgramHeaderTableEntryCount = 0;

        header.SectionHeaderTableEntrySize; // TODO
        header.SectionHeaderTableEntryCount; // TODO
        header.SectionNamesIndex; // TODO

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
        header.SectionHeaderTablePosition = 0; // FIXME

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

        header.ProgramHeaderTableEntrySize; // TODO
        header.ProgramHeaderTableEntryCount = 0;

        header.SectionHeaderTableEntrySize; // TODO
        header.SectionHeaderTableEntryCount = 0; // TODO
        header.SectionNamesIndex; // TODO

        file->write(reinterpret_cast<const char*>(&header), sizeof(header));
    }
    else
    {
        throw Exception::InternalError("Unknown bit mode");
    }
}