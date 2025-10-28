#include "Encoder.hpp"
#include <limits>

size_t Encoder::Section::size() const
{
    return isInitialized ? buffer.size() : reservedSize;
}


Encoder::Encoder::Encoder(const Context& _context, Architecture _arch, BitMode _bits, const Parser::Parser* _parser)
    : context(_context), arch(_arch), bits(_bits), parser(_parser)
{

}

void Encoder::Encoder::Encode()
{
    std::vector<Parser::Section> parsedSections = parser->getSections();

    ResolveConstantsPrePass(parsedSections);

    if (OptimizeOffsets(parsedSections))
        GetOffsets(parsedSections);

    // resolve constant that haven't been resolved yet
    resolveConstants(true);

    relocations.clear();
    EncodeFinal(parsedSections);
}

void Encoder::Encoder::ResolveConstantsPrePass(const std::vector<Parser::Section>& parsedSections)
{
    for (const auto& section : parsedSections)
    {
        for (size_t i = 0; i < section.entries.size(); i++)
        {
            const Parser::SectionEntry& entry = section.entries[i];

            if (std::holds_alternative<Parser::Label>(entry))
            {
                const Parser::Label& label = std::get<Parser::Label>(entry);
                Label lbl;
                lbl.name = label.name;
                lbl.section = section.name;
                lbl.resolved = false;
                lbl.isGlobal = label.isGlobal;
                lbl.isExtern = label.isExtern;
                if (labels.find(lbl.name) == labels.end())
                {
                    labels[lbl.name] = lbl;
                    symbols.push_back(&labels[lbl.name]);
                }
                else
                    throw Exception::SemanticError("Label '" + lbl.name + "' already defined", label.lineNumber, label.column);
            }
            else if (std::holds_alternative<Parser::Constant>(entry))
            {
                const Parser::Constant& constant = std::get<Parser::Constant>(entry);
                Constant c;
                c.name = constant.name;
                c.section = section.name;
                c.expression = constant.value;
                c.resolved = false;
                c.hasPos = constant.hasPos ? HasPos::TRUE : HasPos::UNKNOWN;
                c.isGlobal = constant.isGlobal;
                if (constants.find(constant.name) == constants.end())
                {
                    constants[constant.name] = c;
                    symbols.push_back(&constants[constant.name]);
                }
                else
                    throw Exception::SemanticError("Constant '" + constant.name + "' already defined", constant.lineNumber, constant.column);
            }
        }
    }

    // resolve constant that don't use labels, $ or $$
    resolveConstants(false);
}

void Encoder::Encoder::GetOffsets(std::vector<Parser::Section>& parsedSections)
{
    bytesWritten = 0;
    for (Parser::Section& section : parsedSections)
    {
        sectionStarts[section.name] = bytesWritten;
        currentSection = &section.name;
        sectionOffset = 0;
        for (size_t i = 0; i < section.entries.size(); i++)
        {
            Parser::SectionEntry& entry = section.entries[i];
            
            if (std::holds_alternative<Parser::Instruction::Instruction>(entry))
            {
                Parser::Instruction::Instruction& instruction = std::get<Parser::Instruction::Instruction>(entry);
                const uint64_t size = GetSize(instruction);
                
                sectionOffset += size;
                bytesWritten += size;
            }
            else if (std::holds_alternative<Parser::DataDefinition>(entry))
            {
                const Parser::DataDefinition& dataDefinition = std::get<Parser::DataDefinition>(entry);
                const uint64_t size = GetSize(dataDefinition);

                sectionOffset += size;
                bytesWritten += size;
            }
            else if (std::holds_alternative<Parser::Label>(entry))
            {
                const Parser::Label& label = std::get<Parser::Label>(entry);
                auto it = labels.find(label.name);
                if (it != labels.end())
                {
                    Label& lbl = it->second;
                    lbl.offset = sectionOffset;
                    lbl.resolved = true;
                }
                else
                    throw Exception::InternalError("Label '" + label.name + "' isn't found in constants", label.lineNumber, label.column);
            }
            else if (std::holds_alternative<Parser::Constant>(entry))
            {
                const Parser::Constant& constant = std::get<Parser::Constant>(entry);
                auto it = constants.find(constant.name);
                if (it != constants.end())
                {
                    Constant c = it->second;
                    c.offset = sectionOffset;
                    c.bytesWritten = bytesWritten;

                    constants[constant.name] = c;
                }
                else
                    throw Exception::InternalError("Constant '" + constant.name + "' isn't found in constants", constant.lineNumber, constant.column);
            }
            else if (std::holds_alternative<Parser::Repetition>(entry))
            {
                const Parser::Repetition& repetition = std::get<Parser::Repetition>(entry);
                const Evaluation countEval = Evaluate(repetition.count, bytesWritten, sectionOffset, currentSection);
                const Int128& count128 = countEval.result;

                if (count128 < 0)
                    throw Exception::SemanticError("Repetition count can't be negative", repetition.lineNumber, repetition.column);
                
                if (count128 > std::numeric_limits<uint64_t>::max())
                    throw Exception::InternalError("Repetition count to large for unsigned 64-bit integer", repetition.lineNumber, repetition.column);

                const uint64_t count = static_cast<uint64_t>(count128);
                // TODO
            }
            else if (std::holds_alternative<Parser::Alignment>(entry))
            {
                const Parser::Alignment& alignment = std::get<Parser::Alignment>(entry);
                const Evaluation alignEval = Evaluate(alignment.align, bytesWritten, sectionOffset, currentSection);
                const Int128& align = alignEval.result;

                if (align <= 0)
                    throw Exception::SemanticError("Alignment cannot be zero or lower", alignment.lineNumber, alignment.column);

                const Int128 offset128 = static_cast<Int128>(sectionOffset);
                const Int128 padding128 = (align - (offset128 % align)) % align;
                if (padding128 > std::numeric_limits<uint64_t>::max())
                    throw Exception::InternalError("Padding too large for unsigned 64-bit integer", alignment.lineNumber, alignment.column);

                const uint64_t padding = static_cast<uint64_t>(padding128);
                if (padding > 0)
                {
                    sectionOffset += padding;
                    bytesWritten += padding;
                }
            }  
        }
    }
}

void Encoder::Encoder::EncodeFinal(std::vector<Parser::Section>& parsedSections)
{
    bytesWritten = 0;
    for (Parser::Section& section : parsedSections)
    {
        Section sec;
        sec.name = section.name;
        sec.isInitialized = true;
        sec.align = section.align;
        if (section.name.compare(".bss") == 0)
        {
            sec.isInitialized = false;
        }

        currentSection = &section.name;
        sectionOffset = 0;

        for (size_t i = 0; i < section.entries.size(); i++)
        {
            Parser::SectionEntry& entry = section.entries[i];
            
            if (std::holds_alternative<Parser::Instruction::Instruction>(entry))
            {
                Parser::Instruction::Instruction& instruction = std::get<Parser::Instruction::Instruction>(entry);
                const std::vector<uint8_t> encoded = EncodeInstruction(instruction);
                const size_t size = encoded.size();
                
                if (sec.isInitialized)
                    sec.buffer.insert(sec.buffer.end(), encoded.begin(), encoded.end());
                else
                    sec.reservedSize += size;
                
                sectionOffset += size;
                bytesWritten += size;
            }
            else if (std::holds_alternative<Parser::DataDefinition>(entry))
            {
                const Parser::DataDefinition& dataDefinition = std::get<Parser::DataDefinition>(entry);
                const std::vector<uint8_t> encoded = EncodeData(dataDefinition);
                const size_t size = encoded.size();

                if (sec.isInitialized)
                    sec.buffer.insert(sec.buffer.end(), encoded.begin(), encoded.end());
                else
                    sec.reservedSize += size;

                sectionOffset += size;
                bytesWritten += size;
            }
            else if (std::holds_alternative<Parser::Repetition>(entry))
            {
                const Parser::Repetition& repetition = std::get<Parser::Repetition>(entry);
                const Evaluation countEval = Evaluate(repetition.count, bytesWritten, sectionOffset, currentSection);
                const Int128& count128 = countEval.result;

                if (count128 < 0)
                    throw Exception::SemanticError("Repetition count can't be negative", repetition.lineNumber, repetition.column);
                
                if (count128 > std::numeric_limits<uint64_t>::max())
                    throw Exception::InternalError("Repetition count to large for unsigned 64-bit integer", repetition.lineNumber, repetition.column);

                const uint64_t count = static_cast<uint64_t>(count128);
                // TODO
            }
            else if (std::holds_alternative<Parser::Alignment>(entry))
            {
                const Parser::Alignment& alignment = std::get<Parser::Alignment>(entry);
                const Evaluation alignEval = Evaluate(alignment.align, bytesWritten, sectionOffset, currentSection);
                const Int128& align = alignEval.result;
                
                if (align <= 0)
                    throw Exception::SemanticError("Alignment cannot be zero or lower", alignment.lineNumber, alignment.column);
                
                const Int128 offset128 = static_cast<Int128>(sectionOffset);
                const Int128 padding128 = (align - (offset128 % align)) % align;
                if (padding128 > std::numeric_limits<size_t>::max())
                    throw Exception::InternalError("Padding too large for size_t", alignment.lineNumber, alignment.column);

                const size_t padding = static_cast<size_t>(padding128);
                if (padding > 0)
                {
                    // TODO: really ugly, but works for now
                    std::vector<uint8_t> paddingBytes = EncodePadding(padding);

                    if (sec.isInitialized)
                        sec.buffer.insert(sec.buffer.end(), paddingBytes.begin(), paddingBytes.end());
                    else
                        sec.reservedSize += padding;

                    sectionOffset += padding;
                    bytesWritten += padding;
                }
            }  
        }

        sections.push_back(sec);
    }
}

void Encoder::Encoder::Print() const
{
    for (const auto& section : sections)
    {
        std::cout << section.name << " (aligned to " + std::to_string(section.align) + "):" << std::endl;
        std::cout << "  Size: " << section.size() << std::endl;
    }

    for (const auto& constant : constants)
    {
        const Constant& c = constant.second;
        if (c.resolved)
            std::cout << "Resolved constant ";
        else
            std::cout << "Unresolved constant ";

        if (c.prePass)
            std::cout << "(pre pass): ";

        std::cout << "'" << c.name << "' - at offset " << c.offset << " with bytesWritten " << c.bytesWritten << std::endl;

        if (c.useOffset)
        {
            std::cout << "  offset: " << c.off << " in section '" << c.usedSection << "'" << std::endl;
        }
        else
        {
            std::cout << "  value: " << c.value << std::endl;
        }

        switch (c.hasPos)
        {
            case HasPos::TRUE: std::cout << "  with pos "; break;
            case HasPos::FALSE: std::cout << "  without pos "; break;
            case HasPos::UNKNOWN: default: std::cout << "  with/without (unknown) pos "; break;
        }
        std::cout << std::endl;
    }

    for (const auto& label : labels)
    {
        const Label& l = label.second;
        if (l.resolved)
            std::cout << "Resolved ";
        else
            std::cout << "Unresolved ";

        if (l.isGlobal)
            std::cout << "global label";
        else
            std::cout << "local label";
        
        std::cout << ": '" << l.name << "' in section '" << l.section << "' at offset " << l.offset << std::endl;
    }

    for (const auto& reloc : relocations)
    {
        std::cout << "Relocation in '" << reloc.section << "' at " << reloc.offsetInSection << " using " << (reloc.isExtern ? "extern '" : "'") << reloc.usedSection << "':" << std::endl;
        std::cout << "  Bits: ";
        switch (reloc.size)
        {
            case RelocationSize::Bit8: std::cout << "8"; break;
            case RelocationSize::Bit16: std::cout << "16"; break;
            case RelocationSize::Bit24: std::cout << "24"; break;
            case RelocationSize::Bit32: std::cout << "32"; break;
            case RelocationSize::Bit64: std::cout << "64"; break;
        }
        std::cout << std::endl << "  Type: ";
        switch (reloc.type)
        {
            case RelocationType::Absolute: std::cout << "Absolute"; break;
        }
        std::cout << std::endl << "  Addend: " << reloc.addend << std::endl;
    }
}

#include "x86/Encoder.hpp"

::Encoder::Encoder* ::Encoder::getEncoder(const Context& context, Architecture arch, BitMode bits, const Parser::Parser* parser)
{
    switch (arch)
    {
        case Architecture::x86: return new x86::Encoder(context, arch, bits, parser);
        default: return nullptr;
    }
}
