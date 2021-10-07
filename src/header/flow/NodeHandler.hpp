/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_NODE_HANDLER_H
#define FLOW_NODE_HANDLER_H

#include <components/ValueAware.hpp>
#include <flow/Node.hpp>

namespace flw {

template <typename T, typename... Ts> class NodeHandler : public ValueAware<T> {
  friend class EntityCreator;

public:
  NodeHandler(std::shared_ptr<Node<T, Ts...>> nodeImpl)
      : ValueAware<T>(nodeImpl) {}

  NodeHandler(const NodeHandler<T> &o) : ValueAware<T>(o){};
  NodeHandler<T> &operator==(const NodeHandler<T> &o) {
    static_cast<ValueAware<T> &>(*this) = static_cast<const ValueAware<T> &>(o);
    return *this;
  };
};

} // namespace flw

#endif
