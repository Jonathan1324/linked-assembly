#pragma once

#include <cinttypes>
#include <string>

#include <Architecture.hpp>
#include "../../encoder/encoder.hpp"

namespace COFF {
    struct Data {
    };

    Data create(BitMode bits, Architecture arch, Endianness endianness, Encoded encoded, Parsed parsed, Context& context);
    void write(std::ofstream& out, Endianness endianness, Data& data, Context& context);
}