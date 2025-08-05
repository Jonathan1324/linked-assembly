#pragma once

#include <string>
#include "../Context.hpp"

bool parseArguments(int argc, const char *argv[], std::string& input, std::string& output, bool& debug, const Context& context);
