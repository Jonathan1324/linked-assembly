#pragma once

#include "encoder.hpp"
#include "../architecture/architecture.hpp"

size_t encodeData(const DataDefinition& data, EncodedSection& section, Encoded& encoded, Endianness endianness);