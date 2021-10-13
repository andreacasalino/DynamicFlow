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
  /**
   * @brief triggers the flow update, resetting the nodes who need to be
   * re-evaluated since one or more ancestors changed or the were added before
   * calling any update.
   * The number of threads used to perform the process can be tuned using
   * setThreadsForUpdate
   */
  void updateFlow();

  /**
   * @brief keeps the calling thread busy until the flow update is terminated.
   * In case no update was running returns immediately.
   */
  void waitUpdateComplete(const std::chrono::microseconds &maxWaitTime =
                              std::chrono::microseconds(0)) const;

  /**
   * @return true in case the flow is currently undergoing an update process
   */
  bool isUpdatingFlow() const;

  /**
   * @brief sets the number of threads to use for subsequent calls to
   * updateFlow. Indeed, a parallel region is internally opened to exploit
   * multi-threading to perform all the updates.
   * ATTENTION!!! In case the flow is currently undergoing an update process,
   * the newly set number of threads is ignored and will be considered only for
   * the next invocation to updateFlow.
   * @input the number of threads to perform the update process.
   * If 0 is passed, the maximum number of available threads on this machine is
   * used. If 1 is passed, a single thread will be used, without the need to
   * create a parallel region
   */
  void setThreadsForUpdate(const std::size_t threads = 0);

  /**
   * @return the number of threads that will be used for the update next time
   * updateFlow will be called.
   */
  std::size_t getThreadsForUpdate() const { return threadsForUpdate; };

protected:
  std::atomic_bool busy = false;
  std::atomic<std::size_t> threadsForUpdate = 1;
};

} // namespace flw
