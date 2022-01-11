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

/**
 * @brief A Source is a node at the beginning of the flow.
 * It represents an input, i.e. a value that should be given
 * and may change over the time.
 */
template <typename T>
class Source : public FlowEntity,
               public ValueStorer<T>,
               public DescendantsAware {
  friend class SourceMaker;

public:
  /**
   * @input the new value to reset the stored one.
   */
  void reset(std::unique_ptr<T> newValue) {
    std::scoped_lock<std::mutex> lock(this->valueMtx);
    this->value.reset(std::move(newValue));
    ++this->generations;
  };

protected:
  Source(const std::string &name) : FlowEntity(name){};
};

class SourceMaker {
protected:
  template <typename T, typename... Values>
  Source<T> *makeSource_(Values &&...values) const {
    return new Source<T>(std::forward<Values>(values)...);
  };
};

} // namespace flw
