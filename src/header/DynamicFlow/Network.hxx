/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Network.h>

namespace flw {
template <typename T, template <typename U> class HandlerU>
HandlerU<T> HandlerFinder::find_(const std::string &label) const {
  FlowElementPtr ptr;
  {
    std::scoped_lock guard(flow_lock_);
    auto it = labeled_elements_.find(label);
    if (it == labeled_elements_.end()) {
      if constexpr (std::is_same_v<HandlerU<T>, HandlerSource<T>>) {
        throw Error::make<' '>("Unable to find source named:", label);
      } else {
        throw Error::make<' '>("Unable to find node named:", label);
      }
    }
    ptr = it->second;
  }
  NodeBasePtr<T> res = std::dynamic_pointer_cast<NodeBase<T>>(ptr);
  if (!res) {
    if constexpr (std::is_same_v<HandlerU<T>, HandlerSource<T>>) {
      throw Error::make<' '>("Unable to find source named:", label,
                             "with the requested type");
    } else {
      throw Error::make<' '>("Unable to find node named:", label,
                             "with the requested type");
    }
  }

  UpdateRequiredAware &ref = *const_cast<HandlerFinder *>(this);
  if constexpr (std::is_same_v<HandlerU<T>, HandlerSource<T>>) {
    return HandlerSource<T>{ref, std::dynamic_pointer_cast<Source<T>>(res)};
  } else {
    return Handler<T>{ref, res};
  }
}

template <typename T>
HandlerSource<T> HandlerMaker::makeSource(T initial_value,
                                          const std::string &label) {
  auto src = std::make_shared<Source<T>>(std::move(initial_value));
  std::scoped_lock guard(flow_lock_);
  if (!label.empty() &&
      labeled_elements_.find(label) != labeled_elements_.end()) {
    throw Error::make<' '>("Label `", label, "` was already used");
  }
  sources_.emplace_back(src);
  if (!label.empty()) {
    labeled_elements_.emplace(label, src);
    sources_to_labels_.emplace(src.get(), label);
  }
  return HandlerSource<T>{std::ref(*this), src};
}

template <typename T, typename... As, typename Pred>
Handler<T>
HandlerMaker::makeNode(Pred &&lambda, const Handler<As> &...deps,
                       const std::string &label,
                       std::function<void(const T &)> valueCB,
                       std::function<void(const std::exception &)> errCB) {
  ValueCallBacks<T> cb;
  cb.addOnValue(std::move(valueCB));
  cb.template addOnError<std::exception>(std::move(errCB));
  return this->makeNodeWithErrorsCB<T, As...>(std::forward<Pred>(lambda),
                                              deps..., std::move(cb), label);
}

template <typename T, typename... As, typename... ErrorsT, typename Pred>
Handler<T> HandlerMaker::makeNodeWithErrorsCB(Pred &&lambda,
                                              const Handler<As> &...deps,
                                              ValueCallBacks<T, ErrorsT...> cb,
                                              const std::string &label) {
  if (!label.empty() &&
      labeled_elements_.find(label) != labeled_elements_.end()) {
    throw Error::make<' '>("Label `", label, "` was already used");
  }
  std::tuple<NodeBasePtr<As>...> deps_node;
  packDeps<0, std::tuple<NodeBasePtr<As>...>, As...>(deps_node, deps...);
  auto node =
      Node<T, As...>::make(std::forward<Pred>(lambda), deps_node,
                           std::forward<ValueCallBacks<T, ErrorsT...>>(cb));
  std::scoped_lock guard(flow_lock_);
  nodes_.emplace_back(node);
  if (!label.empty()) {
    labeled_elements_.emplace(label, node);
    nodes_to_labels_.emplace(node.get(), label);
  }
  if (policy.load() == OnNewNodePolicy::IMMEDIATE_UPDATE &&
      node->updatePossible()) {
    node->update();
  } else {
    this->updatePending_.emplace(node.get());
  }
  return Handler<T>{*this, node};
}

template <std::size_t Index, typename TupleT, typename Afront,
          typename... Arest>
void HandlerMaker::packDeps(TupleT &recipient, const Handler<Afront> &dep_front,
                            const Handler<Arest> &...dep_rest) {
  if (this != &dep_front.flow_ref_) {
    throw Error{"All the dependencies must come from the same graph"};
  }
  std::get<Index>(recipient) = dep_front.node_;
  if constexpr (0 < sizeof...(Arest)) {
    packDeps<Index + 1, TupleT, Arest...>(recipient, dep_rest...);
  }
}

template <typename T> void HandlerSource<T>::update(T value) {
  std::scoped_lock guard(update_ref_.flow_lock_);
  if (!source_->update(std::move(value))) {
    return;
  }
  update_ref_.propagateSourceUpdate(*this->source_);
}
} // namespace flw
