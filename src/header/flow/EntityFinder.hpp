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
  /**
   * @input the name of the source to find
   * @return the source in the flow having the passed name
   * @throw In case a source with that name does not exist in this flow
   * @throw In case a source with that name exists, but has a different type other than <T> 
   */
  template <typename T>
  SourceHandler<T> findSource(const std::string &name) const {
    std::lock_guard<std::mutex> creationLock(entityCreationMtx);
    return this->template findSource_<T>(name);
  };

  /**
   * @input the name of the node to find
   * @return the node in the flow having the passed name
   * @throw In case a node with that name does not exist in this flow
   * @throw In case a node with that name exists, but has a different type other than <T> 
   */
  template <typename T> NodeHandler<T> findNode(const std::string &name) const {
    std::lock_guard<std::mutex> creationLock(entityCreationMtx);
    auto it = nodes.find(name);
    if (it == nodes.end()) {
      throw Error("Inexistent");
    }
    std::shared_ptr<ValueStorer<T>> impl =
        std::dynamic_pointer_cast<ValueStorer<T>, FlowEntity>(it->second);
    if (nullptr == impl) {
      throw Error("Wrong type asked");
    }
    return impl;
  };
};

} // namespace flw
