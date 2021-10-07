/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/ValueAware.hpp>
#include <flow/Source.hpp>

namespace flw {

template <typename T> class SourceHandler : public ValueAware<T> {
  friend class SourceHandlerResetter;

public:
  SourceHandler(std::shared_ptr<Source<T>> impl) : ValueAware<T>(impl) {}

  SourceHandler(const SourceHandler<T> &o) : ValueAware<T>(o){};
  SourceHandler<T> &operator==(const SourceHandler<T> &o) {
    static_cast<ValueAware<T> &>(*this) = static_cast<const ValueAware<T> &>(o);
    return *this;
  };

private:
  void reset(std::unique_ptr<T> newValue) {
    Source<T> *sourcePt = dynamic_cast<Source<T> *>(this->storer.get());
    sourcePt->reset(std::move(newValue));
  };
};

class SourceHandlerResetter {
protected:
  template <typename T>
  void reset(std::unique_ptr<T> newValue, SourceHandler<T> &subject) {
    subject.reset(std::move(newValue));
  };
};

} // namespace flw
