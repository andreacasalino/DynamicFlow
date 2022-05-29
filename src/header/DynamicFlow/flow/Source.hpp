/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/components/DescendantsAware.hpp>
#include <DynamicFlow/components/ValueStorer.hpp>
#include <DynamicFlow/flow/FlowEntity.h>

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
  explicit Source(const std::string &name) : FlowEntity(name){};
};

class SourceMaker {
protected:
  template <typename T, typename... Values>
  std::shared_ptr<Source<T>> makeSource_(Values &&...values) const {
    std::shared_ptr<Source<T>> result;
    result.reset(new Source<T>(std::forward<Values>(values)...));
    return result;
  };
};

} // namespace flw
