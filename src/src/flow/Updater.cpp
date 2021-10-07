/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/Updater.h>

namespace flw {
bool Updater::isUpdated() const {
  std::lock_guard<std::mutex> updateLock(updateValuesMtx);
  return requiringUpdate.empty();
}
} // namespace flw
