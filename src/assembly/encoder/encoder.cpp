#include "encoder.hpp"
#include "data.hpp"

void printEncoded(const Encoded& encoded, int indent = 0) {
    std::string tab(indent, '\t');

    for (const auto& [sectionName, section] : encoded.sections) {
        std::cout << tab << "Section: " << sectionName << "\n";

        std::cout << tab << "\tBuffer size: " << section.buffer.size() << " bytes\n";

        if (!section.labels.empty()) {
            std::cout << tab << "\tLabels:\n";
            for (const auto& [labelName, label] : section.labels) {
                std::cout << tab << "\t\tName: " << label.name
                          << ", Offset: " << label.offset
                          << ", Global: " << (label.isGlobal ? "yes" : "no") << "\n";

                if (!label.localLabels.empty()) {
                    std::cout << tab << "\t\tLocal Labels:\n";
                    for (const auto& [localName, localLabel] : label.localLabels) {
                        std::cout << tab << "\t\t\tName: " << localLabel.name
                                  << ", Offset: " << localLabel.offset << "\n";
                    }
                }
            }
        } else {
            std::cout << tab << "\t(No labels)\n";
        }
    }
}

Encoded encode(Parsed& parsed)
{
    Encoded encoded;

    for (const auto& s : parsed.sections)
    {
        EncodedSection& section = encoded.sections[s.name];

        section.name = s.name;

        std::vector<size_t> entryOffsets(s.entries.size());
        size_t currentOffset = 0;

        for (size_t i = 0; i < s.entries.size(); ++i)
        {
            entryOffsets[i] = currentOffset;
            
            if (std::holds_alternative<Instruction>(s.entries[i]))
            {

            }
            else if (std::holds_alternative<DataDefinition>(s.entries[i]))
            {
                DataDefinition data = std::get<DataDefinition>(s.entries[i]);
                size_t padding = (data.alignment > 0) ? 
                    ( (data.alignment - (currentOffset % data.alignment)) % data.alignment ) : 0;

                if (padding != 0)
                {
                    section.buffer.insert(section.buffer.end(), padding, 0x00);
                    currentOffset += padding;
                }

                currentOffset += encodeData(data, section.buffer, encoded);
            }
            else
            {
                //TODO: error - unknown type
            }
        }

        for (const auto& [labelName, label] : s.labels)
        {
            size_t offset = entryOffsets[label.instructionIndex];
            section.labels[label.name].offset = offset;
            
            section.labels[label.name].name = label.name;
            section.labels[label.name].isGlobal = label.isGlobal;

            for (const auto& localLabel : label.localLabels)
            {
                section.labels[label.name].localLabels[localLabel.name].name = localLabel.name;
                section.labels[label.name].localLabels[localLabel.name].offset = entryOffsets[localLabel.instructionIndex];
            }
        }
    }

    printEncoded(encoded);

    return encoded;
}