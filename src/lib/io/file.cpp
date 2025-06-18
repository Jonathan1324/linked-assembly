#include "file.hpp"

#include <Exception.hpp>
#include <cstdio>

std::ifstream openIfstream(const std::string& path)
{
    std::ifstream file(path);
    if (!file.is_open())
        throw Exception::IOError("Couldn't open file " + path);
    return file;
}

std::ofstream openOfstream(const std::string& path, std::ios::openmode mode)
{
    std::ofstream file(path, mode);
    if (!file.is_open())
        throw Exception::IOError("Couldn't open file " + path);
    return file;
}

void deleteFile(const std::string& path)
{
    if (std::remove(path.c_str()) != 0)
        throw Exception::IOError("Couldn't delete file " + path);
}