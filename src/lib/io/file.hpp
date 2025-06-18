#pragma once

#include <fstream>

std::ifstream openIfstream(const std::string& path);
std::ofstream openOfstream(const std::string& path, std::ios::openmode mode);
void deleteFile(const std::string& path);