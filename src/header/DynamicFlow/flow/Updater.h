/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/components/EvaluateCapable.h>

#include <memory>
#include <mutex>
#include <set>

namespace flw {

/**
 * @brief An object aware of the elements in the flow that
 * need to be re-evaluated.
 */
class Updater {
public:
  virtual ~Updater() = default;

  /**
   * @return true in case no elements currently need an update
   */
  bool isUpdated() const;

protected:
  Updater() = default;

  mutable std::mutex updateValuesMtx;
  std::set<EvaluateCapable *> requiringUpdate;

  static std::unique_ptr<std::scoped_lock<std::mutex>>
  makeUpdateValuesMtxLock(Updater &subject) {
    return std::make_unique<std::scoped_lock<std::mutex>>(
        subject.updateValuesMtx);
  };
};

} // namespace flw
