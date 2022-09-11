/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Value.hpp>

#include <algorithm>
#include <set>

namespace flw {
using Values = std::set<const Value *>;

enum class NodeStatus {
  UPDATE_NOT_POSSIBLE, // at least one dependency is unset or stores an
                       // exception
  UPDATE_POSSIBLE,    // all dependencies stores a value and at least one has an
                      // higher epoch than the value stored in this node
  UPDATE_NOT_REQUIRED // all dependencies stores a value, but their epoch is the
                      // same of the value stored in this node
};

class NodeBase {
public:
  virtual ~NodeBase() = default;

  virtual void resetValue() = 0;
  virtual NodeStatus status() const = 0;
  virtual void update() = 0;
  virtual Values dependencies() const = 0;
  virtual const Value *valueUntyped() const = 0;
};
using NodePtr = std::shared_ptr<NodeBase>;

namespace detail {
template <std::size_t N, typename Dep>
class Dependency : virtual public CanGetValue {
public:
  virtual ~Dependency() = default;

  Dependency() = default;

protected:
  const Dep &get() const { return this->CanGetValue::get(*dependency_); }

  const ValueTyped<Dep> *dependency_ = nullptr;
};

template <std::size_t N, typename Dep, typename... Deps>
class Node_ : public Dependency<N, Dep>, public Node_<N + 1, Deps...> {
public:
  Node_() = default;

protected:
  template <typename CB, typename... Inputs>
  void eval(CB &&cb, const Inputs &...inputs) {
    this->Node_<N + 1, Deps...>::eval(std::forward<CB>(cb), inputs...,
                                      this->Dependency<N, Dep>::get());
  }

  void packDependencies(Values &recipient) const {
    recipient.emplace(this->Dependency<N, Dep>::dependency_);
    this->Node_<N + 1, Deps...>::packDependencies(recipient);
  }
};

template <std::size_t N, typename Dep>
class Node_<N, Dep> : public Dependency<N, Dep> {
public:
  Node_() = default;

protected:
  template <typename CB, typename... Inputs>
  void eval(CB &&cb, const Inputs &...inputs) {
    cb(inputs..., this->Dependency<N, Dep>::get());
  }

  void packDependencies(Values &recipient) const {
    recipient.emplace(this->Dependency<N, Dep>::dependency_);
  }
};

template <typename T> class ValueAware {
public:
  virtual ~ValueAware() = default;

  const ValueTyped<T> &getValue() const { return *value_; };
  const Value &getValueUntyped() const { return *value_; };

protected:
  ValueAware(const std::shared_ptr<ValueTyped<T>> &value) { value_ = value; }
  ValueAware(const ValueAware<T> &o) { value_ = o.value_; }

  std::shared_ptr<ValueTyped<T>> value_;
};
} // namespace detail

template <typename Result, typename... Inputs>
using Evaluation = std::function<Result(const Inputs &...)>;

template <typename T, typename Dep, typename... Deps>
class Node : public NodeBase,
             public detail::Node_<0, Dep, Deps...>,
             public detail::ValueAware<T> {
  using EvaluationT = Evaluation<T, Dep, Deps...>;

public:
  Node(EvaluationT &&ev, std::shared_ptr<ValueTyped<T>> value)
      : detail::ValueAware<T>{value},
        evaluation(std::forward<EvaluationT>(ev)) {}

  Node(EvaluationT &&ev)
      : Node(std::forward<EvaluationT>(ev),
             std::make_shared<ValueTypedWithErrors<T>>()) {}

  void resetValue() override { this->value_->reset(); }

  NodeStatus status() const override {
    std::size_t max_epoch_in_dependency = 0;
    for (const auto *dep : this->dependencies()) {
      switch (dep->status()) {
      case ValueStatus::UNSET:
      case ValueStatus::EXCEPTION:
        return NodeStatus::UPDATE_NOT_POSSIBLE;
      case ValueStatus::VALUE:
        max_epoch_in_dependency =
            std::max<std::size_t>(max_epoch_in_dependency, dep->epoch());
        break;
      }
    }
    auto this_epoch = this->value_->epoch();
    return (max_epoch_in_dependency > this_epoch)
               ? NodeStatus::UPDATE_POSSIBLE
               : NodeStatus::UPDATE_NOT_REQUIRED;
  }

  void update() override {
    if (status() != NodeStatus::UPDATE_POSSIBLE) {
      throw Error("Bad call to Node::update");
    }
    auto &recipient = *this->value_;
    this->detail::Node_<0, Dep, Deps...>::eval(
        [&](const Dep &first, const Deps &...inputs) {
          recipient.update([&]() { return evaluation(first, inputs...); });
        });
  }

  template <std::size_t N, typename DepT>
  void bind(const ValueTyped<DepT> &dep) {
    this->detail::Dependency<N, DepT>::dependency_ = &dep;
  }

  Values dependencies() const override {
    Values retVal;
    this->detail::Node_<0, Dep, Deps...>::packDependencies(retVal);
    return retVal;
  }

  const Value *valueUntyped() const override { return &this->getValue(); }

private:
  EvaluationT evaluation;
};
} // namespace flw
