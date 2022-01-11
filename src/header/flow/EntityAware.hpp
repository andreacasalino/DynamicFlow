/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <flow/FlowEntity.h>
#include <flow/SourceHandler.hpp>
#include <map>
#include <memory>
#include <mutex>

namespace flw {

/**
 * @brief An object aware of the flow components
 */
class EntityAware {
public:
  virtual ~EntityAware() = default;

  EntityAware(const EntityAware &) = delete;
  EntityAware &operator==(const EntityAware &) = delete;
  EntityAware(EntityAware &&) = delete;
  EntityAware &operator==(EntityAware &&) = delete;

protected:
  EntityAware() = default;

  template <typename T>
  SourceHandler<T> findSource_(const std::string &name) const {
    auto it = sources.find(FlowName{name});
    if (it == sources.end()) {
      throw Error("Inexistent");
    }
    std::shared_ptr<Source<T>> impl =
        std::dynamic_pointer_cast<Source<T>, FlowEntity>(it->second);
    if (nullptr == impl) {
      throw Error("Wrong type asked");
    }
    return SourceHandler<T>{impl};
  };

  std::map<FlowName, FlowEntityPtr> sources;
  std::map<FlowName, FlowEntityPtr> nodes;

  std::map<FlowName, FlowEntityPtr> allTogether;

  mutable std::mutex entityCreationMtx;

  static std::unique_ptr<std::scoped_lock<std::mutex>>
  makeEntityCreationMtxLock(EntityAware &subject) {
    return std::make_unique<std::scoped_lock<std::mutex>>(
        subject.entityCreationMtx);
  };
};

} // namespace flw
