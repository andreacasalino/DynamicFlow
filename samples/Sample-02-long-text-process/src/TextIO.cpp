#include <Error.h>
#include <TextIO.h>

std::list<std::string> importText(const std::string &filePath) {
  std::ifstream stream(filePath);
  if (!stream.is_open()) {
    throw flw::Error(filePath, " is an inexistent file");
  }

  std::list<std::string> result;
  while (!stream.eof()) {
    result.emplace_back();
    getline(stream, result.back());
  }

  return result;
}

void exportText(const std::list<std::string>& content, const std::string &filePath) {
    std::ofstream stream("Combined");
    if (!stream.is_open()) {
        throw flw::Error(filePath, " is an invalid output file");
    }
    for (const auto &line : content) {
      stream << line << std::endl;
    }
}
