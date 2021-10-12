/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <atomic>
#include <components/ValueOrException.hpp>
#include <mutex>

namespace flw {

template <typename T> class ValueStorer : public ValueOrExceptionAware {
public:
  virtual ~ValueStorer() override = default;

  mutable std::mutex valueMtx;
  ValueOrException<T> value;

  /**
   * @brief It counts the number of times this->value was reset.
   */
  std::atomic<std::size_t> generations = std::atomic<std::size_t>{0};

  bool isValue() const override {
    std::lock_guard<std::mutex> lock(valueMtx);
    return value.isValue();
  };
  bool isException() const override {
    std::lock_guard<std::mutex> lock(valueMtx);
    return value.isException();
  };

  std::exception_ptr getException() const override {
    std::lock_guard<std::mutex> lock(valueMtx);
    return value.getException();
  };

protected:
  ValueStorer() = default;
};

} // namespace flw
