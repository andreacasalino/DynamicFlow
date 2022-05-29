/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/TypeExtractor.hpp>
#include <components/ValueStorer.hpp>

namespace flw {

template <std::size_t Position, typename T> class AncestorAware {
public:
  const ValueStorer<T> *ancestor = nullptr;
};

template <std::size_t Position, typename... Ts> class AncestorsAwareRecurr {
public:
  static const std::size_t AncestorsSize = Position;
};

template <std::size_t Position, typename T, typename... Ts>
class AncestorsAwareRecurr<Position, T, Ts...>
    : public AncestorAware<Position, T>,
      public AncestorsAwareRecurr<Position + 1, Ts...> {};

/**
 * @brief An object linked to some ancestors
 */
template <typename... Ts>
class AncestorsAware : public AncestorsAwareRecurr<0, Ts...> {
public:
  /**
   * @return The Index-th ancestor
   */
  template <std::size_t Index> auto &getAncestor() {
    return static_cast<
        AncestorAware<Index, typename TypeExtractor<Index, Ts...>::Type> &>(
        *this);
  }

  /**
   * @return The Index-th ancestor
   */
  template <std::size_t Index> const auto &getAncestor() const {
    return static_cast<const AncestorAware<
        Index, typename TypeExtractor<Index, Ts...>::Type> &>(*this);
  }

protected:
  AncestorsAware() = default;

  template <typename... Values> void bind(const Values &...ancestors) {
    bind<0, Values...>(ancestors...);
  };

  template <std::size_t Index, typename Value, typename... Values>
  void bind(const Value &value, const Values &...values) {
    bind<Index, Value>(value);
    bind<Index + 1, Values...>(values...);
  };

  template <std::size_t Index, typename Value> void bind(const Value &value) {
    auto &ancestor = getAncestor<Index>();
    ancestor.ancestor = &value;
  };
};
} // namespace flw
