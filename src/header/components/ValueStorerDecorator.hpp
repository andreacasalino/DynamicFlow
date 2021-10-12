/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/ValueStorer.hpp>
#include <flow/FlowEntity.h>

namespace flw {

template <typename T> class ValueStorerDecorator
        : public ValueOrExceptionAware {
  friend class ValueStorerExtractor;

public:
  virtual ~ValueStorerDecorator() = default;

  bool isValue() const override {
    std::lock_guard<std::mutex> lock(this->storer->valueMtx);
    return this->storer->value.isValue();
  };
  bool isException() const override {
    std::lock_guard<std::mutex> lock(this->storer->valueMtx);
    return this->storer->value.isException();
  };

  std::exception_ptr getException() const override {
    std::lock_guard<std::mutex> lock(this->storer->valueMtx);
    return this->storer->value.getException();
  };

  /**
   * @brief Tries to use the value stored by the decorated ValueStorer.
   * @throw In case the decorated storer does not contain a value to use
   * @input A predicate internally called, passing a const reference of
   * the value to use
   */
  template <typename FunctionT> bool useValue(FunctionT action) const {
    std::lock_guard<std::mutex> lock(this->storer->valueMtx);
    if (this->storer->value.isException()) {
      throw Error("This object is an exception and not a value");
    }
    if (!this->storer->value.isValue()) {
      return false;
    }
    action(*this->storer->value.get());
    return true;
  }

  /**
   * @return The number of times the value inside the decorated storer was reset.
   */
  inline std::size_t getGeneration() const { return this->storer->generations; }

  /**
   * @return The name of the decorated storer.
   */
  inline const std::string &getName() const {
    return *dynamic_cast<FlowEntity *>(this->storer.get())->getName().get();
  }

protected:
    ValueStorerDecorator(const std::shared_ptr<ValueStorer<T>> &storer) : storer(storer) {
    if (nullptr == this->storer) {
      throw Error("Empty storer");
    }
  };

  ValueStorerDecorator(const ValueStorerDecorator<T> &o) : ValueStorerDecorator(o.storer){};
  ValueStorerDecorator<T> &operator==(const ValueStorerDecorator<T> &o) {
    storer = o.storer;
    return *this;
  }

  std::shared_ptr<ValueStorer<T>> storer;
};

class ValueStorerExtractor {
protected:
  template <typename ValueAwareT>
  static const auto &extractStorer(const ValueAwareT &subject) {
    return *subject.storer.get();
  };

  template <typename ValueAwareT>
  static auto &extractStorer(ValueAwareT &subject) {
    return *subject.storer.get();
  };
};

/**
 * @brief Does not compiles in case <T> is not copiable.
 * @return The value contained by the stored decorated by the passed object.
 * @throw In case the stored decorated by entity does not contain a value
 */
template <typename T> T copyValue(const ValueStorerDecorator<T> &entity) {
  if (!entity.isValue()) {
    throw Error("Entity named ", entity.getName(),
                " does not contains a value");
  }
  T copy;
  auto cloner = [&copy](const auto &val) { copy = val; };
  entity.useValue(cloner);
  return copy;
}

} // namespace flw
