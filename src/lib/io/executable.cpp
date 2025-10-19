#include "file.hpp"

#include <Exception.hpp>

#include <filesystem>

#ifdef _WIN32
    #include <windows.h>
#elif __APPLE__
    #include <mach-o/dyld.h>
#elif __linux__
    #include <unistd.h>
    #include <limits.h>
#else
    #error "Unsupported OS"
#endif

std::string getExecutablePath()
{
#ifdef _WIN32
    char buffer[MAX_PATH];
    DWORD len = GetModuleFileNameA(NULL, buffer, MAX_PATH);
    if (len == 0 || len == MAX_PATH)
        throw Exception::InternalError("Failed to get executable path (Windows)", -1, -1);
    return std::string(buffer);

#elif __APPLE__
    char buffer[PATH_MAX];
    uint32_t size = sizeof(buffer);
    if (_NSGetExecutablePath(buffer, &size) != 0)
        throw Exception::InternalError("Buffer too small for executable path (macOS)", -1, -1);
    char realPath[PATH_MAX];
    if (realpath(buffer, realPath) == nullptr)
        throw Exception::InternalError("Failed to resolve real path (macOS)", -1, -1);
    return std::string(realPath);

#elif __linux__
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len == -1)
        throw Exception::InternalError("Failed to read /proc/self/exe (Linux)", -1, -1);
    buffer[len] = '\0';
    return std::string(buffer);
#endif
}

std::string getExecutableDir()
{
    return std::filesystem::path(getExecutablePath()).parent_path().string();
}