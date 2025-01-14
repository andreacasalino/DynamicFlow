#include "RunScript.h"

#include <DynamicFlow/Error.h>

namespace flw::sample {
LogDir::LogDir() : path_{LOG_DIR} {
  if (std::filesystem::exists(path_)) {
    std::filesystem::remove_all(path_);
  }
  std::filesystem::create_directories(path_);
}

const std::filesystem::path &LogDir::get() {
  static LogDir guard;
  return guard.path_;
}

void RunScript::runDefaultScript(const std::string &dotFileName) {
  static std::filesystem::path script{DEFAULT_SCRIPT};
  runScript(script, "filename", LogDir::get() / dotFileName);
}

void RunScript::run() {
  auto cmd_str = cmd_.str();
  if (std::system(cmd_str.c_str())) {
    throw Error::make<' '>("Unable to run:", cmd_str);
  }
}
} // namespace flw::sample
