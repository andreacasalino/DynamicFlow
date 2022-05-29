/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/components/DescendantsAware.hpp>
#include <DynamicFlow/components/Evaluator.hpp>
#include <DynamicFlow/flow/FlowEntity.h>

namespace flw {

/**
 * @brief A Node is a node whose value is updated after its
 * ancestors' values are updated.
 */
template <typename T, typename... Ts>
class Node : public FlowEntity,
             public DescendantsAware,
             public Evaluator<T, Ts...> {
  friend class NodeMaker;

protected:
  template <typename... Values>
  Node(const std::string &name,
       const std::function<T(const Ts &...)> &evaluation,
       const Values &...ancestors)
      : Node(name, evaluation) {
    bindSubscribeHandlers<0, Values...>(ancestors...);
  };

  Node(const std::string &name,
       const std::function<T(const Ts &...)> &evaluation)
      : FlowEntity(name), Evaluator<T, Ts...>(evaluation) {}

  template <typename... Values>
  void subscribe(const DescendantsAware &ancestor, const Values &...ancestors) {
    subscribe(ancestor);
    subscribe(ancestors...);
  };

  void subscribe(const DescendantsAware &ancestor) {
    ancestor.descendants.push_back(this);
  };

  template <std::size_t Index, typename Value, typename... Values>
  void bindSubscribeHandlers(const Value &ancestor,
                             const Values &...ancestors) {
    bindSubscribeHandlers<Index, Value>(ancestor);
    bindSubscribeHandlers<Index + 1, Values...>(ancestors...);
  }

  template <std::size_t Index, typename Value>
  void bindSubscribeHandlers(const Value &ancestor) {
    this->template bind<Index>(ancestor);

    const auto *asDescAware = dynamic_cast<const DescendantsAware *>(&ancestor);
    if (nullptr == asDescAware) {
      throw Error("Not a DescendantsAware");
    }
    subscribe(*asDescAware);
  };
};

class NodeMaker {
protected:
  template <typename NodeT, typename... Values>
  std::shared_ptr<NodeT> makeNode_(Values &&...values) const {
    std::shared_ptr<NodeT> result;
    result.reset(new NodeT(std::forward<Values>(values)...));
    return result;
  };
};

} // namespace flw
