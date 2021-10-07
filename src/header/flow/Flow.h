/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <flow/EntityCreator.hpp>
#include <flow/EntityFinder.hpp>
#include <flow/UpdateCapable.h>

namespace flw {

class Flow : public EntityCreator, public EntityFinder, public UpdateCapable {
public:
  Flow() = default;
};
} // namespace flw
