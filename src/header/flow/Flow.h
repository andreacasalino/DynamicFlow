/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <flow/EntityCreator.hpp>
#include <flow/EntityFinder.hpp>
#include <flow/UpdaterFlow.h>
#include <flow/UpdaterSources.h>

namespace flw {

/**
 * @brief A flow which is only updatable manipulating the existent entities
 */
class FlowUpdatable : public EntityFinder,
                      public UpdaterSources,
                      public UpdaterFlow {
public:
  FlowUpdatable() = default;

  template <typename... UpdateInputs>
  void updateSourcesAndFlow(UpdateInputs &&...inputs) {
    updateSources(std::forward<UpdateInputs>(inputs)...);
    updateFlow();
  }
};

/**
 * @brief A complete flow which is not only updatable, but also
 * extendable by adding new sources and nodes
 */
class Flow : public EntityCreator, public FlowUpdatable {
public:
  ~Flow() override = default;

  Flow() = default;

  Flow &operator=(Flow &&o) noexcept {
    this->absorb(std::move(o));
    return *this;
  };
};
} // namespace flw
