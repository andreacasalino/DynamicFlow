/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/EvaluateCapable.h>
#include <mutex>
#include <set>

namespace flw {

class Updater {
public:
  bool isUpdated() const;

protected:
  Updater() = default;

  mutable std::mutex updateValuesMtx;
  std::set<EvaluateCapable *> requiringUpdate;
};

} // namespace flw
