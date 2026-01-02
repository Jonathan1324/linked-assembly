#pragma once

#include <filesystem>
#include <Exception.hpp>

struct Context
{
    WarningManager* warningManager;
    std::string filename;
    std::vector<std::filesystem::path> include_paths;
};
