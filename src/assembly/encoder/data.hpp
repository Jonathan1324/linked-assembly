#pragma once

#include "encoder.hpp"
#include <Architecture.hpp>

size_t encodeData(const DataDefinition& data, EncodedSection& section, Encoded& encoded, Endianness endianness, Context& context);