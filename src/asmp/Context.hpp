#pragma once

#include <Exception.hpp>

struct Context
{
    WarningManager* warningManager;
    std::string filename;
};