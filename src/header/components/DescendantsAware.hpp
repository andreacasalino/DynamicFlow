/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/EvaluateCapable.h>
#include <list>

namespace flw {

class DescendantsAware {
public:
  mutable std::list<EvaluateCapable *> descendants;

protected:
  DescendantsAware() = default;
};

} // namespace flw
