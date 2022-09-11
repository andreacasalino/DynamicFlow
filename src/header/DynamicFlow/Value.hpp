/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Error.h>
#include <DynamicFlow/TypeTraits.h>

#include <atomic>
#include <functional>
#include <limits.h>
#include <memory>
#include <mutex>
#include <ostream>
#include <variant>

namespace flw {
namespace detail {
static constexpr std::size_t MAX_SIZE_T =
    std::numeric_limits<std::size_t>::max();
} // namespace detail

enum class ValueStatus { UNSET, VALUE, EXCEPTION };

class Value {
public:
  virtual ~Value() = default;

  virtual ValueStatus status() const = 0;

  /**
   * @brief throws any stored exception, if any is present.
   */
  virtual void reThrow() const = 0;

  /**
   * @brief empty this value by deleting any exception/value stored, leaving the
   * object into an unset state.
   */
  virtual void reset() = 0;

  /**
   * @brief if a value is stored and can be converted to a string, the converted
   * version of the value is returned. Otherwise, an empty string is returned.
   * Attention!! This feature made internally use of concepts and is actually
   * enabled only in case C++20 is set. Otherwise, you can still compile this
   * code, but an empty string will be always returned.
   */
  virtual std::string toString() const = 0;

  /**
   * @return the number of times this object was modified, i.e. the number of
   * times a new expection or a new value was stored.
   */
  std::size_t epoch() const { return value_epoch_.load(); }

protected:
  Value() = default;

  void incrementEpoch() {
    auto val = value_epoch_.load();
    if (val == detail::MAX_SIZE_T) {
      val = 0;
    } else {
      ++val;
    }
    value_epoch_.store(val);
  }

private:
  std::atomic<std::size_t> value_epoch_ = 0;
};

namespace detail {
template <typename... Errors>
using ReThrownable_ =
    ReThrownable<std::exception, Error, UnsetValueError, Errors...>;

template <typename T, typename... Errors>
using Value_ = std::variant<std::unique_ptr<T>, ReThrownable_<Errors...>>;

class CanGetValue;
} // namespace detail

template <typename T> class ValueTyped : public Value {
  friend class detail::CanGetValue;

public:
  /**
   * @brief evaluates the passed predicate in order to generate a new value to
   * store, or a new expection in case something went wrong.
   */
  virtual void update(const std::function<T()> &generator) = 0;

  using OnNewValue = std::function<void(const T &)>;
  /**
   * @brief You can't access directly the value stored in this object. What you
   * can do, is to register with this method a call back that is called passing
   * the a new generated value whenever one is generated.
   */
  template <typename Predicate> void onNewValueCallBack(Predicate &&cb) {
    std::lock_guard<std::mutex> lock(cb_mtx_);
    onNewValue_.emplace(std::forward<Predicate>(cb));
  }

  /**
   * @brief Similar to onNewValueCallBack(...), but for registering a call back
   * called everytime a new exception is stored.
   */
  using OnNewException = std::function<void(const Value &)>;
  template <typename Predicate> void onNewExceptionCallBack(Predicate &&cb) {
    std::lock_guard<std::mutex> lock(cb_mtx_);
    onNewException_.emplace(std::forward<Predicate>(cb));
  }

protected:
  virtual const T &get() const = 0;

  std::mutex cb_mtx_;
  std::optional<OnNewValue> onNewValue_;
  std::optional<OnNewException> onNewException_;
};

namespace detail {
class CanGetValue {
public:
  virtual ~CanGetValue() = default;

protected:
  template <typename T> const T &get(const ValueTyped<T> &subject) const {
    return subject.get();
  }
};
} // namespace detail

/**
 * @brief An object that may store a value to use or an exception
 * explaining why the value computation was not possible.
 * It can't store both a value and an exception, but
 * before initialization neither a value nor an exception is actually stored.
 */
template <typename T, typename... Errors>
class ValueTypedWithErrors : public ValueTyped<T> {
public:
  ValueTypedWithErrors() {
    detail::ReThrownable_<Errors...> r;
    r.handle([]() { throw UnsetValueError{}; });
    value_ = std::move(r);
  }

  ValueStatus status() const override {
    std::scoped_lock<std::mutex> lock(value_mtx_);
    struct Visitor {
      mutable ValueStatus retVal;

      void operator()(const std::unique_ptr<T> &v) const {
        retVal = ValueStatus::VALUE;
      }

      void operator()(const detail::ReThrownable_<Errors...> &r) const {
        try {
          r.reThrow();
        } catch (const UnsetValueError &) {
          retVal = ValueStatus::UNSET;
        } catch (...) {
          retVal = ValueStatus::EXCEPTION;
        }
      }
    } visitor;
    std::visit(visitor, value_);
    return visitor.retVal;
  }

  void reThrow() const override {
    std::scoped_lock<std::mutex> lock(value_mtx_);
    const auto *as_thrownable =
        std::get_if<detail::ReThrownable_<Errors...>>(&value_);
    if (as_thrownable) {
      as_thrownable->reThrow();
    }
  }

  void reset() override {
    std::scoped_lock<std::mutex> lock(value_mtx_);
    value_ = nullptr;
    detail::ReThrownable_<Errors...> r;
    r.handle([]() { throw UnsetValueError{}; });
    detail::Value_<T, Errors...> new_val = std::move(r);
    value_ = std::move(new_val);
  }

  std::string toString() const override {
    auto *as_val = std::get_if<std::unique_ptr<T>>(&value_);
    if (as_val == nullptr) {
      return "null";
    }
    auto serialization = flw::to_string(*as_val->get());
    return serialization ? serialization.value() : "non-serializable";
  }

  void update(const std::function<T()> &generator) override {
    const T *new_value = nullptr;
    {
      std::scoped_lock<std::mutex> lock(value_mtx_);
      detail::ReThrownable_<Errors...> handler;
      std::unique_ptr<T> maybe_result;
      if (handler.handle([&generator, &maybe_result]() {
            maybe_result = std::make_unique<T>(generator());
          })) {
        value_ = std::move(handler);
      } else {
        if constexpr (has_comparison_operator) {
          auto *previous_val = std::get_if<std::unique_ptr<T>>(&value_);
          if ((previous_val != nullptr) &&
              (*previous_val->get() == *maybe_result.get())) {
            return;
          }
        }
        new_value = maybe_result.get();
        value_ = std::move(maybe_result);
      }
    }
    this->incrementEpoch();
    {
      std::lock_guard<std::mutex> locK(this->cb_mtx_);
      if (new_value == nullptr) {
        if (this->onNewException_) {
          this->onNewException_.value()(*this);
        }
      } else {
        if (this->onNewValue_) {
          this->onNewValue_.value()(*new_value);
        }
      }
    }
  }

private:
  const T &get() const override {
    std::scoped_lock<std::mutex> lock(value_mtx_);
    const auto *as_val = std::get_if<std::unique_ptr<T>>(&value_);
    if (!as_val) {
      throw UnsetValueError{};
    }
    return *as_val->get();
  }

  static constexpr bool has_comparison_operator =
      HasComparisonOperator<T>::value;

  mutable std::mutex value_mtx_;
  detail::Value_<T, Errors...> value_;
};
} // namespace flw
