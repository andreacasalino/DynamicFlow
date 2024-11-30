/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <sstream>
#include <stdexcept>
#include <variant>

namespace flw {
namespace detail {
template <char Sep, typename First, typename... Rest>
void merge_(std::stringstream &stream, const First &first,
            const Rest &...args) {
  stream << first;
  if constexpr (sizeof...(Rest) != 0) {
    if constexpr (0 < Sep) {
      stream << Sep;
    }
    merge_<Sep, Rest...>(stream, args...);
  }
}

template <char Sep, typename... ARGS> std::string merge(const ARGS &...args) {
  std::stringstream stream;
  merge_<Sep>(stream, args...);
  return stream.str();
}
} // namespace detail

class Error : public std::runtime_error {
public:
  explicit Error(const std::string &what);

  template <char Sep, typename... Args>
  static Error make(const Args &...args) {
    return Error(detail::merge<Sep>(args...));
  }
};

} // namespace flw
