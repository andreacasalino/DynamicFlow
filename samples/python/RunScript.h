#pragma once

#include <iostream>
#include <sstream>
#include <string>

namespace flw::sample {
namespace detail {
void appendArgs_(std::ostringstream &stream);

template <typename T>
void appendArgs_(std::ostringstream &stream, const T &first) {
  stream << ' ' << first;
}

template <typename T, typename... Args>
void appendArgs_(std::ostringstream &stream, const T &first,
                 const Args &...others) {
  appendArgs_(stream, first);
  appendArgs_(stream, others...);
}
} // namespace detail

template <typename... Args>
void runPython(const std::string &script_name, const Args &...args) {
  std::ostringstream cmd;
  cmd << "python3 " << script_name;
  detail::appendArgs_(cmd, args...);
  std::cout << "running `" << cmd.str() << '`' << std::endl;
  int retVal = std::system(cmd.str().c_str());
}

void runShowGraph(const std::string &dot_file_to_render);
} // namespace flw::sample
