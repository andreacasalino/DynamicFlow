/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <components/DescendantsAware.hpp>
#include <components/ValueStorer.hpp>
#include <flow/FlowEntity.h>

namespace flw {

template <typename T>
class Source : public FlowEntity,
               public ValueStorer<T>,
               public DescendantsAware {
  friend class SourceMaker;

public:
  void reset(std::unique_ptr<T> newValue) {
    std::lock_guard<std::mutex> lock(this->valueMtx);
    this->value.reset(std::move(newValue));
    ++this->generations;
  };

protected:
  Source(const std::string &name) : FlowEntity(name){};
};

class SourceMaker {
protected:
  template <typename T, typename... Values>
  Source<T> *makeNode(Values &&...values) const {
    return new Source<T>(std::forward<Values...>(std::move(values))...);
  };
};

} // namespace flw
