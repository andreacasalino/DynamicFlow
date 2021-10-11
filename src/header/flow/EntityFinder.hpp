/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <flow/EntityAware.hpp>
#include <flow/NodeHandler.hpp>

namespace flw {

class EntityFinder : virtual public EntityAware {
public:
  template <typename T>
  SourceHandler<T> findSource(const std::string &name) const {
    std::lock_guard<std::mutex> creationLock(entityCreationMtx);
    return this->template findSource_<T>(name);
  };

  template <typename T, typename... Ts>
  NodeHandler<T, Ts...> findNode(const std::string &name) const {
    std::lock_guard<std::mutex> creationLock(entityCreationMtx);
    auto it = nodes.find(name);
    if (it == nodes.end()) {
      throw Error("Inexistent");
    }
    std::shared_ptr<Node<T, Ts...>> impl =
        std::dynamic_pointer_cast<Node<T, Ts...>, FlowEntity>(it->second);
    if (nullptr == impl) {
      throw Error("Wrong type asked");
    }
    return impl;
  };
};

} // namespace flw
