/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/ValueStorerDecorator.hpp>
#include <flow/Updater.h>

namespace flw {

class UpdaterFlow : virtual public Updater, private ValueStorerExtractor {
public:
  void updateFlow();

  void waitUpdateComplete(const std::chrono::microseconds &maxWaitTime =
                              std::chrono::microseconds(0)) const;

  bool isUpdatingFlow() const;

  void setThreadsForUpdate(const std::size_t threads = 0);

protected:
  std::atomic_bool busy = false;
  std::atomic<std::size_t> threadsForUpdate = 1;
};

} // namespace flw
