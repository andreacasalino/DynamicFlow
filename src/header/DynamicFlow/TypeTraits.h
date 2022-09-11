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
static constexpr bool CPLUPLUS_20_ENABLED = __cplusplus >= 202002;

#if __cplusplus >= 202002
namespace detail {
template <typename T>
concept HasComparisonOperator_ = requires(const T &a, const T &b) {
  { a == b } -> std::convertible_to<bool>;
};
} // namespace detail
#endif

template <typename T> struct HasComparisonOperator {
  static constexpr bool value =
#if __cplusplus >= 202002
      detail::HasComparisonOperator_<T>
#else
      false
#endif
      ;
};

#if __cplusplus >= 202002
namespace detail {
template <typename T>
concept HasStreamOperator = requires(std::ostream &recipient,
                                     const T &subject) {
  { operator<<(recipient, subject) } -> std::convertible_to<std::ostream &>;
};

template <typename T>
concept CanBeStringConvertible = requires(const T &subject) {
  { std::to_string(subject) } -> std::convertible_to<std::string>;
};
} // namespace detail
#endif

template <typename T> std::optional<std::string> to_string(const T &subject) {
#if __cplusplus >= 202002
  if constexpr (detail::HasStreamOperator<T>) {
    std::ostringstream result;
    result << subject;
    return result.str();
  }
  if constexpr (detail::CanBeStringConvertible<T>) {
    return std::to_string(subject);
  }
#endif
  return std::nullopt;
}
} // namespace flw
