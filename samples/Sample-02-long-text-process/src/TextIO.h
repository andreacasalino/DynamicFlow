#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>

namespace flw::sample {
class FileNotFound : public std::runtime_error {
public:
  FileNotFound(const std::string &filePath)
      : std::runtime_error{filePath + " : is an invalid file name"} {};
};

std::unique_ptr<std::ifstream> make_in_stream(const std::string &filePath);
std::unique_ptr<std::ofstream> make_out_stream(const std::string &filePath);

std::list<std::string> importText(const std::string &fileName);

void exportText(const std::list<std::string> &content,
                const std::string &filePath);

} // namespace flw::sample
