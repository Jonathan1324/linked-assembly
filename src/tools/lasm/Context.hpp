#pragma once

#include <Exception.hpp>
#include <StringPool.hpp>

struct Context
{
    WarningManager* warningManager;
    std::string filename;
    StringPool* stringPool;
};