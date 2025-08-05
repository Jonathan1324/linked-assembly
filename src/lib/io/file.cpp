#include "file.hpp"

#include <Exception.hpp>
#include <cstdio>

std::istream* openIstream(const std::string& path)
{
    if (path == "-")
        return &std::cin;

    std::ifstream* file = new std::ifstream(path);
    if (!file->is_open())
    {
        delete file;
        throw Exception::IOError("Couldn't open file " + path);
    }
    return file;
}

std::ostream* openOstream(const std::string& path, std::ios::openmode mode)
{
    if (path == "-")
        return &std::cout;

    std::ofstream* file = new std::ofstream(path, mode);
    if (!file->is_open())
    {
        delete file;
        throw Exception::IOError("Couldn't open file " + path);
    }
    return file;
}

void deleteFile(const std::string& path)
{
    if (std::remove(path.c_str()) != 0)
        throw Exception::IOError("Couldn't delete file " + path);
}