/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <flow/UpdateCapable.h>
#include <omp.h>

namespace flw {

namespace {

bool process(std::set<EvaluateCapable *> &toUpdate) {
  bool isBlocked = true;
  auto it = toUpdate.begin();
  while (it != toUpdate.end()) {
    auto res = (*it)->evaluate();
    if ((EvaluationResult::SUCCESS == res) ||
        (EvaluationResult::BLOCKING_EXCEPTION == res)) {
      it = toUpdate.erase(it);
      isBlocked = false;
    } else {
      // EvaluationResult::NOT_READY
      ++it;
    }
  }
  return isBlocked;
}

void updateNodesSerial(std::set<EvaluateCapable *> &toUpdate) {
  while (!toUpdate.empty()) {
    if (process(toUpdate)) {
      // throw Error("Something went wrong with the Flow update");
      return;
    }
  }
}

bool areAllTrue(const std::vector<bool> &flags) {
  for (const auto flag : flags) {
    if (!flag) {
      return false;
    }
  }
  return true;
}

void updateNodesParallel(std::set<EvaluateCapable *> &toUpdate,
                         const std::size_t threads) {
  std::vector<std::set<EvaluateCapable *>> queues;
  queues.resize(threads);
  std::vector<bool> blockedFlags(threads, false);
  bool stop = false;
#pragma omp parallel num_threads(static_cast <const int>(threads))
  {
    int thID = omp_get_thread_num();
    if (0 == thID) {
      while (!toUpdate.empty()) {
#pragma omp barrier
        blockedFlags[thID] = process(queues[thID]);
#pragma omp barrier
        if (areAllTrue(blockedFlags)) {
          break;
        }
      }
      stop = true;
#pragma omp barrier
    } else {
      while (true) {
        if (stop) {
          break;
        }
#pragma omp barrier
        blockedFlags[thID] = process(queues[thID]);
#pragma omp barrier
      }
    }
  }
}
} // namespace

std::set<EvaluateCapable *> UpdateCapable::computeUpdateRequired(
    const std::set<EvaluateCapable *> &initialNodes) {
  std::set<EvaluateCapable *> open = initialNodes, close;
  while (!open.empty()) {
    auto *front = *open.begin();
    open.erase(open.begin());
    DescendantsAware *impl = dynamic_cast<DescendantsAware *>(front);
    for (auto *d : impl->descendants) {
      open.emplace(d);
    }
    close.emplace(front);
  }
  return close;
}

void UpdateCapable::updateNodes(std::set<EvaluateCapable *> toUpdate) {
  if (toUpdate.empty()) {
    return;
  }
  const std::size_t threads = threadsForUpdate;
  if (1 == threads) {
    updateNodesSerial(toUpdate);
  } else {
    updateNodesParallel(toUpdate, threads);
  }
}

bool UpdateCapable::isBusy() const { return busy; }

namespace {

template <typename Condition> void waitInfinite(Condition condition) {
  while (condition()) {
  }
}

template <typename Condition>
void waitFinite(Condition condition,
                const std::chrono::microseconds &maxWaitTime) {
  std::chrono::microseconds elapsed(0);
  while (true) {
    auto tic = std::chrono::high_resolution_clock::now();
    if (!condition()) {
      return;
    }
    elapsed += std::chrono::duration_cast<std::chrono::microseconds>(
        std::chrono::high_resolution_clock::now() - tic);
    if (elapsed.count() >= maxWaitTime.count()) {
      return;
    }
  }
}

std::size_t getThreadsAvailability() {
  std::size_t max_threads;
#pragma omp parallel
  { max_threads = omp_get_thread_num(); }
  return max_threads;
}

} // namespace

void UpdateCapable::waitUpdateComplete(
    const std::chrono::microseconds &maxWaitTime) const {
  if (0 == maxWaitTime.count()) {
    waitInfinite([this]() { return this->isBusy(); });
  } else {
    waitFinite([this]() { return this->isBusy(); }, maxWaitTime);
  }
}

static const std::size_t MAX_THREADS = getThreadsAvailability();
void UpdateCapable::setThreadsForUpdate(const std::size_t threads) {
  if (0 == threads) {
    threadsForUpdate = MAX_THREADS;
  }
  threadsForUpdate = threads;
}
} // namespace flw