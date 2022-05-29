/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/components/ValueStorerDecorator.hpp>
#include <DynamicFlow/flow/Source.hpp>

namespace flw {

template <typename T> class SourceHandler : public ValueStorerDecorator<T> {
  friend class SourceHandlerResetter;

public:
  ~SourceHandler() override = default;

  explicit SourceHandler(std::shared_ptr<Source<T>> impl)
      : ValueStorerDecorator<T>(impl) {}

  SourceHandler<T> &operator==(const SourceHandler<T> &o) {
    static_cast<ValueStorerDecorator<T> &>(*this) =
        static_cast<const ValueStorerDecorator<T> &>(o);
    return *this;
  };

private:
  void reset(std::unique_ptr<T> newValue) const {
    auto *sourcePt = dynamic_cast<Source<T> *>(this->storer.get());
    sourcePt->reset(std::move(newValue));
  };
};

class SourceHandlerResetter {
protected:
  template <typename T>
  void reset(std::unique_ptr<T> newValue, SourceHandler<T> &subject) const {
    subject.reset(std::move(newValue));
  };
};

} // namespace flw
