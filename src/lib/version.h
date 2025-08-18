#pragma once

#include "cpp/cpp.h"

HCB

#ifndef VERSION
#define VERSION commit
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define VERSION_STR STR(VERSION)

void printVersion();

HCE