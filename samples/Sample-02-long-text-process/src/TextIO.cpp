#include <TextIO.h>
#include <sstream>

namespace flw::sample {
namespace {
template <typename StreamT>
std::unique_ptr<StreamT> make_stream(const std::filesystem::path &filePath) {
  auto result = std::make_unique<StreamT>(filePath);
  if (!result->is_open()) {
    throw FileNotFound{filePath};
  }
  return result;
}
} // namespace

std::unique_ptr<std::ifstream> make_in_stream(const std::filesystem::path &filePath) {
  return make_stream<std::ifstream>(filePath);
}

std::unique_ptr<std::ofstream> make_out_stream(const std::filesystem::path &filePath) {
  return make_stream<std::ofstream>(filePath);
}

std::list<std::string> importText(const std::filesystem::path &fileName) {
  auto stream = make_in_stream(std::filesystem::path{ASSET_FOLDER} / fileName);

  std::list<std::string> result;
  while (!stream->eof()) {
    result.emplace_back();
    getline(*stream, result.back());
  }

  return result;
}

void exportText(const std::list<std::string> &content,
                const std::filesystem::path &filePath) {
  auto stream = make_out_stream(filePath);
  for (const auto &line : content) {
    *stream << line << std::endl;
  }
}
} // namespace flw::sample
