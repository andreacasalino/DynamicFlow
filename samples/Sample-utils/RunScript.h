#pragma once

#include <filesystem>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>

namespace flw::sample {
struct LogDir {
  static const std::filesystem::path &get();

private:
  LogDir();

  std::filesystem::path path_;
};

struct RunScript {
  template <typename... ARGS>
  static void runScript(const std::filesystem::path &location, ARGS &&...args) {
    RunScript tmp{};
    tmp.cmd_ << PYTHON_CMD;
    tmp.cmd_ << ' ';
    tmp.cmd_ << location;
    tmp.addArg_(args...);
    tmp.run();
  }

  static void runDefaultScript(const std::string &dotFileName);

private:
  template <typename Front, typename... ARGS>
  void addArg_(const std::string_view &name, Front &&front_val,
               ARGS &&...args) {
    cmd_ << ' ';
    cmd_ << "--";
    cmd_ << name;
    cmd_ << ' ';
    cmd_ << front_val;
    if constexpr (0 < sizeof...(ARGS)) {
      addArg_<ARGS...>(args...);
    }
  }

  RunScript() = default;

  void run();

  std::stringstream cmd_;
};

} // namespace flw::sample
