/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <string.h>

namespace flw {
template <std::size_t Position, typename T, typename... Ts>
struct TypeExtractor {
  using Type = typename TypeExtractor<Position - 1, Ts...>::Type;
};

template <typename T, typename... Ts> struct TypeExtractor<0, T, Ts...> {
  using Type = T;
};
} // namespace flw
