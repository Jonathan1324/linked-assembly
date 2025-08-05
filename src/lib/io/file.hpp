#pragma once

#include <fstream>
#include <string>

std::istream* openIstream(const std::string& path);
std::ostream* openOstream(const std::string& path, std::ios::openmode mode = std::ios::out);
void deleteFile(const std::string& path);

std::string getExecutablePath();
std::string getExecutableDir();