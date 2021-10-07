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

template <typename T> class ValueAware {
  friend class ValueAwareStorerExtractor;

public:
  virtual ~ValueAware() = default;

  bool isValue() const {
    std::lock_guard<std::mutex> lock(this->storer->valueMtx);
    return this->storer->value.isValue();
  };
  bool isException() const {
    std::lock_guard<std::mutex> lock(this->storer->valueMtx);
    return this->storer->value.isException();
  };

  std::exception_ptr getException() const {
    std::lock_guard<std::mutex> lock(this->storer->valueMtx);
    return this->storer->value.getException();
  };

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

  inline std::size_t getGeneration() const { return this->storer->generations; }

  inline const std::string &getName() const {
    return *dynamic_cast<FlowEntity *>(this->storer.get())->getName().get();
  }

protected:
  ValueAware(const std::shared_ptr<ValueStorer<T>> &storer) : storer(storer) {
    if (nullptr == this->storer) {
      throw Error("Empty storer");
    }
  };

  ValueAware(const ValueAware<T> &o) : ValueAware(o.storer){};
  ValueAware<T> &operator==(const ValueAware<T> &o) {
    storer = o.storer;
    return *this;
  }

  std::shared_ptr<ValueStorer<T>> storer;
};

class ValueAwareStorerExtractor {
  template <typename ValueAwareT>
  static const auto &extractStorer(const ValueAwareT &subject) {
    return *subject.storer.get();
  };
};

template <typename T> T copyValue(const ValueAware<T> &entity) {
  if (!entity.isValue()) {
    throw Error("Entity named ", entity.getName(),
                " does not contains a value");
  }
  T copy;
  auto cloner = [&copy](const auto &val) { copy = val; };
  entity.useValue(cloner);
  return copy;
}

template <typename T>
std::unique_ptr<T> copyValuePtr(const ValueAware<T> &entity) {
  if (!entity.isValue()) {
    throw Error("Entity named ", entity.getName(),
                " does not contains a value");
  }
  std::unique_ptr<T> copy;
  auto cloner = [&copy](const auto &val) { copy = std::make_unique<T>(val); };
  entity.useValue(cloner);
  return std::move(copy);
}
} // namespace flw
