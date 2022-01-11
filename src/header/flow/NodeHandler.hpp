/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/ValueStorerDecorator.hpp>
#include <flow/Node.hpp>

namespace flw {

template <typename T> class NodeHandler : public ValueStorerDecorator<T> {
public:
  explicit NodeHandler(std::shared_ptr<ValueStorer<T>> impl)
      : ValueStorerDecorator<T>(impl) {}

  NodeHandler(const NodeHandler<T> &o) : ValueStorerDecorator<T>(o){};
  NodeHandler<T> &operator==(const NodeHandler<T> &o) {
    static_cast<ValueStorerDecorator<T> &>(*this) =
        static_cast<const ValueStorerDecorator<T> &>(o);
    return *this;
  };
};

} // namespace flw
