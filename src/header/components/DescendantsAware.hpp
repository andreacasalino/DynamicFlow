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

/**
 * @brief An object aware of its descendants, i.e.
 * objects whose internal expression needs this one
 * to be evaluated
 */
class DescendantsAware {
public:
  virtual ~DescendantsAware() = default;

  mutable std::list<EvaluateCapable *> descendants;

protected:
  DescendantsAware() = default;
};

} // namespace flw
