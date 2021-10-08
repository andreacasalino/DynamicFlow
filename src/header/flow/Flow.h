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

class Flow : public EntityCreator,
             public EntityFinder,
             public UpdaterSources,
             public UpdaterFlow {
public:
  Flow() = default;

  template <typename... UpdateInputs>
  void updateSourcesAndFlow(UpdateInputs &&...inputs) {
    updateSources(std::forward<UpdateInputs>(inputs)...);
    updateFlow();
  }
};
} // namespace flw
