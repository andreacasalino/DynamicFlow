#pragma once

#include <fstream>
#include <list>
#include <string>

std::list<std::string> importText(const std::string &filePath);

void exportText(const std::list<std::string>& content, const std::string &filePath);
