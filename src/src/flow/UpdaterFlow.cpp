/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <DynamicFlow/flow/UpdaterFlow.h>
#include <omp.h>
#include <vector>

namespace flw {

namespace {

void spread(std::vector<std::set<EvaluateCapable *>> &recipient,
            std::set<EvaluateCapable *> &donator) {
  std::size_t k = 0;
  for (const auto &e : donator) {
    recipient[k].emplace(e);
    ++k;
    if (recipient.size() == k) {
      k = 0;
    }
  }
  donator.clear();
};

void gather(std::set<EvaluateCapable *> &recipient,
            std::vector<std::set<EvaluateCapable *>> &donator) {
  for (auto &d : donator) {
    for (const auto &e : d) {
      recipient.emplace(e);
    }
    d.clear();
  }
};

// returns true when a deadlock is reached,
// i.e. none of the element was processe
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

void updateNodesParallel(std::set<EvaluateCapable *> &toUpdate,
                         const std::size_t threads) {
  std::vector<std::set<EvaluateCapable *>> queues;
  queues.resize(threads);
  std::atomic_bool isDeadlock;
  bool stop = false;
#pragma omp parallel num_threads(static_cast <const int>(threads))
  {
    int thID = omp_get_thread_num();
    if (0 == thID) {
      while (!toUpdate.empty()) {
        spread(queues, toUpdate);
        isDeadlock = true;
#pragma omp barrier
        if (!process(queues[thID])) {
          isDeadlock = false;
        }
#pragma omp barrier
        gather(toUpdate, queues);
        if (isDeadlock) {
          break;
        }
      }
      stop = true;
#pragma omp barrier
    } else {
      while (true) {
#pragma omp barrier
        if (stop) {
          break;
        }
        if (!process(queues[thID])) {
          isDeadlock = false;
        }
#pragma omp barrier
      }
    }
  }
}
} // namespace

void UpdaterFlow::updateFlow() {
  std::scoped_lock<std::mutex> updateLock(updateValuesMtx);
  busy = true;
  if (!requiringUpdate.empty()) {
    const std::size_t threads = threadsForUpdate;
    if (1 == threads) {
      updateNodesSerial(requiringUpdate);
    } else {
      updateNodesParallel(requiringUpdate, threads);
    }
  }
  busy = false;
}

bool UpdaterFlow::isUpdatingFlow() const { return busy; }

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

void UpdaterFlow::waitUpdateComplete(
    const std::chrono::microseconds &maxWaitTime) const {
  if (0 == maxWaitTime.count()) {
    waitInfinite([this]() { return this->isUpdatingFlow(); });
  } else {
    waitFinite([this]() { return this->isUpdatingFlow(); }, maxWaitTime);
  }
}

static const std::size_t MAX_THREADS = getThreadsAvailability();
void UpdaterFlow::setThreadsForUpdate(const std::size_t threads) {
  if (0 == threads) {
    threadsForUpdate = MAX_THREADS;
    return;
  }
  threadsForUpdate = threads;
}
} // namespace flw
