/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <optional>
#include <ostream>
#include <sstream>
#include <string>

namespace flw {
template <typename T>
concept HasComparisonOperator = requires(const T &a, const T &b) {
  { a == b } -> std::convertible_to<bool>;
};

template <typename T>
concept HasStreamOperator = requires(std::ostream &recipient,
                                     const T &subject) {
  { recipient << subject } -> std::convertible_to<std::ostream &>;
};

template <typename T>
concept CanBeStringConvertible = requires(const T &subject) {
  { std::to_string(subject) } -> std::convertible_to<std::string>;
};
} // namespace flw
