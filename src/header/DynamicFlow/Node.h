/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Value.hxx>

#include <algorithm>
#include <memory>

namespace flw {
class FlowElementResettable;
using FlowElementResettablePtr = std::shared_ptr<FlowElementResettable>;

class FlowElement {
public:
  virtual ~FlowElement() = default;

  struct SerializationInfo {
    std::size_t generation;

    /**
     * @brief if a value is stored and can be converted to a string, the
     * converted version of the value is returned. Otherwise, an empty string is
     * returned.
     */
    std::string value_serialized;
  };
  virtual SerializationInfo serializationInfo() const = 0;

  const auto &dependants() const { return dependants_; }

  virtual bool hasValue() const = 0;

protected:
  FlowElement() = default;

  static void link(FlowElement &parent, FlowElementResettablePtr dependant) {
    parent.dependants_.push_back(dependant);
  }

private:
  std::vector<FlowElementResettablePtr> dependants_;
};
using FlowElementPtr = std::shared_ptr<FlowElement>;

class Resettable {
public:
  virtual ~Resettable() = default;

  virtual void reset() = 0;

  const auto &dependencies() const { return dependencies_; }
  bool updatePossible() const;

  virtual void update() = 0;

protected:
  std::vector<FlowElementResettablePtr> dependencies_;
};

class FlowElementResettable : virtual public FlowElement, public Resettable {
public:
  FlowElementResettable() = default;
};

template <typename T> class NodeBase : virtual public FlowElement {
public:
  SerializationInfo serializationInfo() const final;

  bool hasValue() const final { return value_->getValue(); }

  auto value() const { return value_; }

protected:
  NodeBase(ValueBasePtr<T> val) : value_{val} {}

private:
  ValueBasePtr<T> value_;
};
template <typename T> using NodeBasePtr = std::shared_ptr<NodeBase<T>>;

template <typename T, typename... As>
class Node : public NodeBase<T>, public FlowElementResettable {
  static_assert(
      0 < sizeof...(As),
      "nodes should have at least one dependency ... otherwise are sources!!!");

public:
  using Lambda = std::function<T(const As &...)>;
  using Ancestors = std::tuple<ValueBasePtr<As>...>;

  template <typename... ErrorsT>
  static std::shared_ptr<Node<T, As...>>
  make(Lambda &&lam, const std::tuple<NodeBasePtr<As>...> &deps,
       ValueCallBacks<T, ErrorsT...> &&cb);

  void reset() final { this->valueT_->reset(); }

  void update() final;

private:
  template <typename... ErrorsT>
  Node(Lambda &&lam, const std::tuple<NodeBasePtr<As>...> &deps,
       ValueCallBacks<T, ErrorsT...> &&cb);

  template <std::size_t Index>
  static void link(std::shared_ptr<Node<T, As...>> subject,
                   const std::tuple<NodeBasePtr<As>...> &deps);

  std::shared_ptr<ValueT<T>> valueT_;
  Lambda lambda_;
  Ancestors ancestors_;
};

template <typename T> class Source : public NodeBase<T> {
public:
  Source(T &&initial_value)
      : NodeBase<T>{
            std::make_shared<ValueSource<T>>(std::forward<T>(initial_value))} {
    source_ = std::dynamic_pointer_cast<ValueSource<T>>(this->value());
  }

  bool update(T &&val);

private:
  std::shared_ptr<ValueSource<T>> source_;
};
} // namespace flw
