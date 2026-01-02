#include "file.hpp"

#include <Exception.hpp>
#include <cstdio>

#ifdef _WIN32
#include <fcntl.h>
#include <io.h>
#endif

std::istream* openIstream(const std::string& path, std::ios::openmode mode)
{
    if (path == "-")
    {
#ifdef _WIN32
        if (mode & std::ios::binary)
        {
            _setmode(_fileno(stdin), _O_BINARY);
        }
#endif
        return &std::cin;
    }

    std::ifstream* file = new std::ifstream(path, mode);
    if (!file->is_open())
    {
        delete file;
        throw Exception::IOError("Couldn't open file " + path, -1, -1);
    }
    return file;
}

std::ostream* openOstream(const std::string& path, std::ios::openmode mode)
{
    if (path == "-")
    {
#ifdef _WIN32
        if (mode & std::ios::binary)
        {
            _setmode(_fileno(stdout), _O_BINARY);
        }
#endif
        return &std::cout;
    }

    std::ofstream* file = new std::ofstream(path, mode);
    if (!file->is_open())
    {
        delete file;
        throw Exception::IOError("Couldn't open file " + path, -1, -1);
    }
    return file;
}

void deleteFile(const std::string& path)
{
    if (std::remove(path.c_str()) != 0)
        throw Exception::IOError("Couldn't delete file " + path, -1, -1);
}