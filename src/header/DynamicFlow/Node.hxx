/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Node.h>
#include <DynamicFlow/TypeTraits.h>

#include <sstream>

namespace flw {
template <typename T>
FlowElement::SerializationInfo NodeBase<T>::serializationInfo() const {
  SerializationInfo res{value_->getGeneration(), "null"};
  if (const T *ptr = value_->getValue(); ptr) {
    if constexpr (CanBeStringConvertible<T>) {
      res.value_serialized = std::to_string(*ptr);
    } else if constexpr (HasStreamOperator<T>) {
      std::stringstream buffer;
      buffer << *ptr;
      res.value_serialized = buffer.str();
    } else {
      res.value_serialized = "non-serializable";
    }
  }
  return res;
}

template <typename T, typename... As>
template <std::size_t Index>
void Node<T, As...>::link(std::shared_ptr<Node<T, As...>> subject,
                          const std::tuple<NodeBasePtr<As>...> &deps) {
  if constexpr (Index < sizeof...(As)) {
    auto &parent_ref = *std::get<Index>(deps);
    FlowElement::link(parent_ref, subject);
    std::get<Index>(subject->ancestors_) = parent_ref.value();
    link<Index + 1>(subject, deps);
  }
}

template <typename T, typename... As>
template <typename... ErrorsT>
Node<T, As...>::Node(Lambda &&lam, const std::tuple<NodeBasePtr<As>...> &deps,
                     ValueCallBacks<T, ErrorsT...> &&cb)
    : NodeBase<T>{std::make_shared<Value<T, ErrorsT...>>(
          std::forward<ValueCallBacks<T, ErrorsT...>>(cb))},
      lambda_{std::forward<Lambda>(lam)} {
  valueT_ = std::dynamic_pointer_cast<ValueT<T>>(this->value());
}

namespace detail {
template <std::size_t Index, typename AncestorsT>
void linkDependencies(std::vector<FlowElementResettablePtr> &recipient,
                      const AncestorsT &ancestors) {
  if constexpr (Index < std::tuple_size<AncestorsT>::value) {
    auto ptr = std::get<Index>(ancestors);
    if (auto ptr_cast = std::dynamic_pointer_cast<FlowElementResettable>(ptr);
        ptr_cast) {
      recipient.emplace_back(ptr_cast);
    }
    linkDependencies<Index + 1>(recipient, ancestors);
  }
}
} // namespace detail

template <typename T, typename... As>
template <typename... ErrorsT>
std::shared_ptr<Node<T, As...>>
Node<T, As...>::make(Lambda &&lam, const std::tuple<NodeBasePtr<As>...> &deps,
                     ValueCallBacks<T, ErrorsT...> &&cb) {
  std::shared_ptr<Node<T, As...>> res;
  res.reset(
      new Node<T, As...>{std::forward<Lambda>(lam), deps,
                         std::forward<ValueCallBacks<T, ErrorsT...>>(cb)});
  detail::linkDependencies<0>(res->dependencies_, deps);
  link<0>(res, deps);
  return res;
}

namespace detail {
template <std::size_t Index, typename T, typename Ancestors, typename Lambda,
          typename... Args>
T gen_val(const Ancestors &ancestors, const Lambda &lam, const Args &...args) {
  if constexpr (Index < std::tuple_size<Ancestors>::value) {
    auto &ref = std::get<Index>(ancestors);
    return gen_val<Index + 1, T>(ancestors, lam, args..., *ref->getValue());
  } else {
    return lam(args...);
  }
}
} // namespace detail

template <typename T, typename... As> void Node<T, As...>::update() {
  this->valueT_->update(
      [this]() { return detail::gen_val<0, T>(ancestors_, lambda_); });
}

template <typename T> bool Source<T>::update(T &&val) {
  if constexpr (HasComparisonOperator<T>) {
    if (*source_->getValue() == val) {
      return false;
    }
  }
  source_->update(std::forward<T>(val));
  return true;
}
} // namespace flw
