/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <flow/EntityAware.hpp>
#include <flow/NodeHandler.hpp>
#include <set>

namespace flw {

class UpdateCapable : virtual public EntityAware, public SourceHandlerResetter {
public:
  template <typename... UpdateInputs>
  void updateFlow(UpdateInputs &&...inputs) {
    std::unique_ptr<std::lock_guard<std::mutex>> creationLock =
        std::make_unique<std::lock_guard<std::mutex>>(entityCreationMtx);
    std::lock_guard<std::mutex> updateLock(updateValuesMtx);
    busy = true;
    std::set<EvaluateCapable *> toUpdate;
    {
      updateSource(toUpdate, std::forward<UpdateInputs>(inputs)...);
      toUpdate = computeUpdateRequired(toUpdate);
      creationLock.reset();
    }
    updateNodes(toUpdate);
    busy = false;
  }

  void waitUpdateComplete(const std::chrono::microseconds &maxWaitTime =
                              std::chrono::microseconds(0)) const;

  bool isBusy() const;

  void setThreadsForUpdate(const std::size_t threads = 0);

protected:
  template <typename T, typename... UpdateInputs>
  void updateSource(std::set<EvaluateCapable *> &toUpdate,
                    const std::string &source_name,
                    std::unique_ptr<T> new_value, UpdateInputs &&...remaining) {
    this->template updateSource<T>(toUpdate, source_name, std::move(new_value));
    updateSource(toUpdate, std::forward<UpdateInputs>(remaining)...);
  }
  template <typename T>
  void updateSource(std::set<EvaluateCapable *> &toUpdate,
                    const std::string &source_name,
                    std::unique_ptr<T> new_value) {
    SourceHandler<T> handler = this->template findSource_<T>(source_name);
    this->reset(std::move(new_value), handler);
    Source<T> *impl = dynamic_cast<Source<T> *>(handler.storer.get());
    for (auto *d : impl->descendants) {
      toUpdate.emplace(d);
    }
  }

  std::set<EvaluateCapable *>
  computeUpdateRequired(const std::set<EvaluateCapable *> &initialNodes);

  void updateNodes(std::set<EvaluateCapable *> toUpdate);

  mutable std::mutex updateValuesMtx;
  std::atomic_bool busy = false;
  std::atomic<std::size_t> threadsForUpdate = 1;
};

} // namespace flw
