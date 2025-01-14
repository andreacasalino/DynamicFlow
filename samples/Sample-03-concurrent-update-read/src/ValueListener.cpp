#include <ValueListener.h>

namespace flw::sample {
namespace {
struct Timer {
  static Timer &get() {
    static Timer res;
    return res;
  }

  std::chrono::nanoseconds now() const {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
        std::chrono::system_clock::now() - start_tp);
  }

private:
  Timer() = default;

  std::chrono::system_clock::time_point start_tp =
      std::chrono::system_clock::now();
};
} // namespace

std::jthread ValueListener::startAndTransfer() {
  return std::jthread{[this](std::stop_token token) {
    while (!token.stop_requested()) {
      int val = last_value_.load();

      time_t timestamp;
      time(&timestamp);
      stream_ << std::chrono::duration_cast<std::chrono::milliseconds>(
                     Timer::get().now())
                     .count()
              << "[ms]: " << val << std::endl;

      std::this_thread::sleep_for(period_);
    }
  }};
}
} // namespace flw::sample
