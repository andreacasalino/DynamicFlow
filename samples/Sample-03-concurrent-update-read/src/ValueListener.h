#pragma once

#include <filesystem>
#include <atomic>
#include <fstream>
#include <thread>
#include <chrono>

namespace flw::sample {
struct ValueListener {
  ValueListener(const std::filesystem::path& logFile, const std::chrono::milliseconds& period)
  : period_{period}
  , stream_{logFile} {}

  void onNewValue(int val) {
    last_value_.store(val);
  }

  std::jthread startAndTransfer();

private:
  std::chrono::milliseconds period_;
  std::ofstream stream_;
  std::atomic<int> last_value_;
};
} // namespace flw::sample
