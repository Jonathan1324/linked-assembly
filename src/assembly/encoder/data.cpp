#include "data.hpp"

size_t evaluate(std::__1::string value, EncodedSection& section, Encoded& encoded)
{
    (void)section;
    (void)encoded;
    //TODO
    return std::stoull(value, nullptr, 0);
}

size_t encodeData(const DataDefinition& data, EncodedSection& section, Encoded& encoded)
{
    sectionBuffer& buffer = section.buffer;
    size_t bytesWritten = 0;

    size_t typeSize = 1;
    if (data.type == "db" || data.type == "resb") typeSize = 1;
    else if (data.type == "dw" || data.type == "resw") typeSize = 2;
    else if (data.type == "dd" || data.type == "resd") typeSize = 4;
    else if (data.type == "dq" || data.type == "resq") typeSize = 8;
    else if (data.type == "dt" || data.type == "rest") typeSize = 10;
    else
    {
        std::cerr << "Unsupported data type (line " << data.lineNumber << "): " << data.type << std::endl;
    }

    if (data.reserved)
    {
        //TODO
        size_t count = evaluate(data.values[0], section, encoded);
        buffer.insert(buffer.end(), count * typeSize, 0x00);
        bytesWritten = count * typeSize;
    }
    else
    {
        for (const auto& valStr : data.values)
        {
            unsigned long val = evaluate(valStr, section, encoded);

            if (typeSize == 10)
            {
                // TODO: fix
                std::cerr << "dt doesn't work yet." << std::endl;
                for (size_t i = 0; i < 10; ++i)
                    buffer.push_back(0x00);
                bytesWritten += 10;
            }
            else
            {
                // little endian
                for (size_t i = 0; i < typeSize; ++i)
                    buffer.push_back(static_cast<unsigned char>((val >> (8 * i)) & 0xFF));
                bytesWritten += typeSize;
            }
        }
    }

    return bytesWritten;
}