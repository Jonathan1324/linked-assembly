#pragma once

#include <vector>

template<typename T>
void writeToBuffer(std::vector<unsigned char>& buffer, const T& value)
{
    const unsigned char* data = reinterpret_cast<const unsigned char*>(&value);
    buffer.insert(buffer.end(), data, data + sizeof(T));
}