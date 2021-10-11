/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <Error.h>
#include <memory>

namespace flw {

class ValueOrExceptionAware {
public:
  virtual bool isValue() const = 0;
  virtual bool isException() const = 0;

  virtual std::exception_ptr getException() const = 0;

protected:
  ValueOrExceptionAware() = default;
};

template <typename T> class ValueOrException : public ValueOrExceptionAware {
public:
  /** @brief nullptr c'tor
   */
  ValueOrException() = default;

  ValueOrException(std::unique_ptr<T> newValue) {
    this->reset(std::move(newValue));
  }

  void reset(std::unique_ptr<T> newValue = nullptr) {
    this->exception = std::exception_ptr();
    this->value = std::move(newValue);
  };

  ValueOrException(const std::exception_ptr &exception) {
    this->reset(exception);
  };

  void resetException(const std::exception_ptr &exception) {
    this->exception = exception;
    this->value.reset();
  };

  inline bool isValue() const override { return (nullptr != this->value); };
  inline bool isException() const override {
    return (nullptr != this->exception);
  };

  inline std::exception_ptr getException() const override {
    return this->exception;
  };

  const T *get() const { return this->value.get(); };

  T *get() { return this->value.get(); };

private:
  std::unique_ptr<T> value;
  std::exception_ptr exception;
};

inline bool operator==(std::nullptr_t, const ValueOrExceptionAware &o) {
  return !o.isValue();
};
inline bool operator==(const ValueOrExceptionAware &o, std::nullptr_t) {
  return !o.isValue();
};
} // namespace flw
