#pragma once

#include <fstream>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
#include <filesystem>

namespace flw::sample {
class FileNotFound : public std::runtime_error {
public:
  FileNotFound(const std::filesystem::path &filePath)
      : std::runtime_error{filePath.string() + " : is an invalid file name"} {};
};

std::unique_ptr<std::ifstream> make_in_stream(const std::filesystem::path &filePath);
std::unique_ptr<std::ofstream> make_out_stream(const std::filesystem::path &filePath);

std::list<std::string> importText(const std::filesystem::path &fileName);

void exportText(const std::list<std::string> &content,
                const std::filesystem::path &filePath);

} // namespace flw::sample
