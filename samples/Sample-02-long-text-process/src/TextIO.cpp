#include <TextIO.h>
#include <sstream>

namespace flw::sample {
namespace {
template <typename StreamT>
std::unique_ptr<StreamT> make_in_stream(const std::string &filePath) {
  auto result = std::make_unique<StreamT>(filePath);
  if (!result->is_open()) {
    throw FileNotFound{filePath};
  }
  return result;
}
} // namespace

std::unique_ptr<std::ifstream> make_in_stream(const std::string &filePath) {
  return make_in_stream<std::ifstream>(filePath);
}

std::unique_ptr<std::ofstream> make_out_stream(const std::string &filePath) {
  return make_in_stream<std::ofstream>(filePath);
}

std::list<std::string> importText(const std::string &fileName) {
  std::stringstream streamPath;
  streamPath << SAMPLE_PATH << "assets/" << fileName;

  auto stream = make_in_stream(streamPath.str());

  std::list<std::string> result;
  while (!stream->eof()) {
    result.emplace_back();
    getline(*stream, result.back());
  }

  return result;
}

void exportText(const std::list<std::string> &content,
                const std::string &filePath) {
  auto stream = make_out_stream(filePath);
  for (const auto &line : content) {
    *stream << line << std::endl;
  }
}
} // namespace flw::sample
