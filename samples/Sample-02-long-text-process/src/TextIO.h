#pragma once

#include <DynamicFlow/components/ValueOrException.hpp>
#include <fstream>
#include <list>
#include <string>

std::list<std::string> importText(const std::string &filePath);

void exportText(const std::list<std::string> &content,
                const std::string &filePath);

flw::ValueOrException<std::ofstream>
make_out_stream(const std::string &filePath);
