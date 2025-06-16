#pragma once

#include <unordered_map>
#include <string>

constexpr const unsigned char octal = 0xFE;
constexpr const unsigned char hex = 0xFF;

static const std::unordered_map<char, unsigned char> escapeSign = 
{
    {'a', 0x07},
    {'b', 0x08},
    {'f', 0x0C},
    {'n', 0x0A},
    {'r', 0x0D},
    {'t', 0x09},
    {'v', 0x0B},
    {'\\', '\\'},
    {'\'', '\''},
    {'\"', '\"'},
    {'?', 0x3F},
    {'0', octal},
    {'x', hex}
};