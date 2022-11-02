#pragma once

#include <list>
#include <string>

namespace flw::sample {
std::size_t countSpaces(const std::string &line);

std::list<std::string> parseWords(const std::string &line);
} // namespace flw::sample