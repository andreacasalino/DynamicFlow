/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <sstream>
#include <stdexcept>

namespace flw {
/** @brief A runtime error that can be raised when using any object in flw::
 */
class Error : public std::runtime_error {
public:
  Error(const std::string &what);

  template <typename T, typename... Args>
  Error(T front, Args... args) : Error(Error::merge(front, args...)){};

private:
  template <typename... Args> static std::string merge(Args... args) {
    std::stringstream stream;
    Error::merge_(stream, args...);
    return stream.str();
  }

  template <typename T, typename... Args>
  static void merge_(std::stringstream &stream, T piece, Args... args) {
    Error::merge_(stream, piece);
    Error::merge_(stream, args...);
  }

  template <typename T> static void merge_(std::stringstream &stream, T piece) {
    stream << piece;
  }
};
} // namespace flw
