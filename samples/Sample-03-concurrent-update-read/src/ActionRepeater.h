#pragma once

#include <atomic>
#include <chrono>
#include <thread>

namespace flw::sample {
template <typename Action>
void repeat_action(Action action, std::atomic_bool &condition,
                   const std::chrono::microseconds &sample_time) {
  while (condition.load()) {
    action();
    std::this_thread::sleep_for(sample_time);
  }
}
} // namespace flw::sample
