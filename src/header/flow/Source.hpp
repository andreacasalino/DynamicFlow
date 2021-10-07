/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_SOURCE_H
#define FLOW_SOURCE_H

#include <components/DescendantsAware.hpp>
#include <components/ValueStorer.hpp>
#include <flow/FlowEntity.h>

namespace flw {

template <typename T>
class Source : public FlowEntity,
               public ValueStorer<T>,
               public DescendantsAware {
  friend class EntityCreator;

public:
  void reset(std::unique_ptr<T> newValue) {
    std::lock_guard<std::mutex> lock(this->valueMtx);
    this->value.reset(std::move(newValue));
    ++this->generations;
  };

protected:
  Source(const std::string &name) : FlowEntity(name){};
};

} // namespace flw

#endif
