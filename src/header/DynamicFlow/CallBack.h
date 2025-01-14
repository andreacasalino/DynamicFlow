/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <functional>

namespace flw {

template <typename T> class CallBack {
public:
  CallBack() = default;

  CallBack(const CallBack&) = delete;
  CallBack& operator=(const CallBack&) = delete;
  
  CallBack(CallBack&&) = default;
  CallBack& operator=(CallBack&&) = default;

  template <typename Pred> void add(Pred &&pred) {
    if (cb_) {
      cb_ = [prev = std::move(cb_), pred = std::forward<Pred>(pred)](const T& val) {
        pred(val);
        prev(val);
      };
    } else {
      cb_ = std::forward<Pred>(pred);
    }
  }

  void operator()(const T &ref) const {
    if (cb_) {
      cb_(ref);
    }
  }

private:
  std::function<void(const T &)> cb_;
};

} // namespace flw
