#pragma once

#include <cinttypes>
#include <string>

#include "../../architecture/architecture.hpp"
#include "../../encoder/encoder.hpp"

namespace COFF {
    struct Data {
    };

    Data create(BitMode bits, Architecture arch, Endianness endianness, Encoded encoded, Parsed parsed);
    void write(std::ofstream& out, Endianness endianness, Data& data);
}