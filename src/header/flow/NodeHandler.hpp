/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/ValueAware.hpp>
#include <flow/Node.hpp>

namespace flw {

template <typename T, typename... Ts> class NodeHandler : public ValueAware<T> {
public:
  template <typename... Values>
  NodeHandler(const std::string &name,
              const std::function<T(const Ts &...)> &evaluation,
              const Values &...handlers)
      : ValueAware<T>(std::make_shared<Node<T, Ts...>>(
            name, evaluation, getStorer(handlers)...)){};

  NodeHandler(const NodeHandler<T> &o) : ValueAware<T>(o){};
  NodeHandler<T> &operator==(const NodeHandler<T> &o) {
    static_cast<ValueAware<T> &>(*this) = static_cast<const ValueAware<T> &>(o);
    return *this;
  };
};

} // namespace flw
