/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/CallBack.h>
#include <DynamicFlow/Error.h>

#include <functional>
#include <memory>
#include <optional>
#include <ostream>

namespace flw {
template <typename T> class ValueBase {
public:
  virtual ~ValueBase() = default;

  ValueBase() = default;

  std::size_t getGeneration() const { return generation_; }

  const T *getValue() const {
    return value_.has_value() ? &value_.value() : nullptr;
  }

protected:
  std::size_t generation_ = 0;
  std::optional<T> value_;
};
template <typename T> using ValueBasePtr = std::shared_ptr<ValueBase<T>>;

template <typename T> class ValueSource : public ValueBase<T> {
public:
  ValueSource(T &&val_initial) { this->update(std::forward<T>(val_initial)); }

  void update(T &&val) {
    ++this->generation_;
    this->value_.emplace(std::forward<T>(val));
  }
};

template <typename T, typename... ErrorsT> class ValueCallBacks {
public:
  ValueCallBacks() = default;

  /**
   * @brief You can't access directly the value stored in this object. What
   * you can do, is to register with this method a call back that is called
   * passing the a new generated value whenever one is generated.
   */
  template <typename Pred> ValueCallBacks &addOnValue(Pred &&pred) {
    onValue_.add(std::forward<Pred>(pred));
    return *this;
  }

  /**
   * @brief Similar to onNewValueCallBack(...), but for registering a call back
   * called everytime a particular exception is throwned.
   */
  template <typename ErrorT, typename Pred>
  ValueCallBacks &addOnError(Pred &&pred) {
    auto &ref = std::get<CallBack<ErrorT>>(onError_);
    ref.add(std::forward<Pred>(pred));
    return *this;
  }

  ValueCallBacks extract() { return std::move(*this); }

protected:
  void onValue(const T &val) { onValue_(val); }

  template <typename ErrorT> void onError(const ErrorT &e) {
    auto &ref = std::get<CallBack<ErrorT>>(onError_);
    ref(e);
  }

private:
  CallBack<T> onValue_;
  std::tuple<CallBack<ErrorsT>..., CallBack<std::exception>> onError_;
};

template <typename T> class ValueT : public ValueBase<T> {
public:
  void reset() { this->value_.reset(); }

  virtual void update(std::function<T()> pred) = 0;
};

template <typename T, typename... ErrorsT>
class Value : public ValueT<T>, public ValueCallBacks<T, ErrorsT...> {
public:
  Value(ValueCallBacks<T, ErrorsT...> cb)
      : ValueCallBacks<T, ErrorsT...>{std::move(cb)} {}

  void update(std::function<T()> pred) final;

private:
  template <typename ErrorFirst, typename... ErrorsRest>
  void update_(const std::function<T()> &pred);
};

} // namespace flw
