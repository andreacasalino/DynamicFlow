#include <DynamicFlow/Error.h>
#include <TextIO.h>
#include <sstream>

std::list<std::string> importText(const std::string &filePath) {
  std::stringstream streamPath;
  streamPath << SAMPLE_PATH << filePath;

  std::ifstream stream(streamPath.str());
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

flw::ValueOrException<std::ofstream>
make_out_stream(const std::string &filePath) {
  flw::ValueOrException<std::ofstream> result;
  try {
    result.reset(std::make_unique<std::ofstream>(filePath));
    if (!result.get()->is_open()) {
      throw flw::Error(filePath, " is an inexistent file");
    }
  } catch (...) {
    result.resetException(std::current_exception());
  }
  return result;
}

void exportText(const std::list<std::string> &content,
                const std::string &filePath) {
  auto stream = make_out_stream(filePath);
  if (stream.isException()) {
    std::rethrow_exception(stream.getException());
  }
  for (const auto &line : content) {
    *stream.get() << line << std::endl;
  }
}
