#include <Error.h>
#include <TextImport.h>

std::list<std::string> importText(const std::string &filePath) {
  std::ifstream stream(filePath);
  if (!stream.is_open()) {
    throw flw::Error("Inexistent file");
  }

  std::list<std::string> result;
  while (!stream.eof()) {
    result.emplace_back();
    getline(stream, result.back());
  }

  return result;
}
