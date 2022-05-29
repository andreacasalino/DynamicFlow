/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/components/ValueOrException.hpp>

#include <atomic>
#include <mutex>

namespace flw {

template <typename T> class ValueStorer : public ValueOrExceptionAware {
public:
  mutable std::mutex valueMtx;
  ValueOrException<T> value;

  /**
   * @brief It counts the number of times this->value was reset.
   */
  std::atomic<std::size_t> generations = std::atomic<std::size_t>{0};

  bool isValue() const override {
    std::scoped_lock<std::mutex> lock(valueMtx);
    return value.isValue();
  };
  bool isException() const override {
    std::scoped_lock<std::mutex> lock(valueMtx);
    return value.isException();
  };

  std::exception_ptr getException() const override {
    std::scoped_lock<std::mutex> lock(valueMtx);
    return value.getException();
  };
};

} // namespace flw
