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
  virtual ~ValueOrExceptionAware() = default;

  /**
   * @return true when this object stores a non nullptr value.
   */
  virtual bool isValue() const = 0;
  /**
   * @return true when this object stores an exception.
   */
  virtual bool isException() const = 0;

  /**
   * @return the stored exception. Returns nullptr when the 
   * object is not currently storing an exception
   */
  virtual std::exception_ptr getException() const = 0;

protected:
  ValueOrExceptionAware() = default;
};


/**
 * @brief An object that may store a value to use or an exception
 * explaining why the value computation was not possible.
 * It can't store both a value and an exception, but
 * before initialization neither a value nor an exception is actually stored.
 */
template <typename T> class ValueOrException : public ValueOrExceptionAware {
public:
  /** 
   * @brief After construction neither a value nor an exception is actually stored.
   */
  ValueOrException() = default;

  /**
   * @brief The object is built passing the initial value to store
   */
  ValueOrException(std::unique_ptr<T> newValue) {
    this->reset(std::move(newValue));
  }

  /**
   * @input The the new value to store.
   */
  void reset(std::unique_ptr<T> newValue = nullptr) {
    this->exception = std::exception_ptr();
    this->value = std::move(newValue);
  };

  /**
   * @brief The object is built passing the initial exception to store
   */
  ValueOrException(const std::exception_ptr &exception) {
    this->reset(exception);
  };

  /**
   * @input The the new exception to store.
   */
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

  /**
   * @return the value currently stored. Might be a nullptr is the object is not currently a value
   */
  const T *get() const { return this->value.get(); };

  /**
   * @return the value currently stored. Might be a nullptr is the object is not currently a value
   */
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
