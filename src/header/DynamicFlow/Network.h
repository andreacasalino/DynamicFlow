/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Node.hxx>

#include <atomic>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace flw {
class HandlerMaker;
class HandlerFinder;
class FlowBase;
class UpdateRequiredAware;

template <typename T> class Handler {
  friend class HandlerMaker;
  friend class HandlerFinder;

public:
  Handler(const Handler &) = default;
  Handler &operator=(const Handler &) = default;

  Handler(Handler &&) = default;
  Handler &operator=(Handler &&) = default;

protected:
  Handler(FlowBase &flow, NodeBasePtr<T> node) : flow_ref_{flow}, node_{node} {}

private:
  FlowBase &flow_ref_;
  NodeBasePtr<T> node_;
};

template <typename T> class HandlerSource : public Handler<T> {
  friend class HandlerMaker;
  friend class HandlerFinder;

public:
  void update(T value);

private:
  HandlerSource(UpdateRequiredAware &flow, std::shared_ptr<Source<T>> source)
      : Handler<T>{flow, source}, update_ref_{flow}, source_{source} {}

  std::shared_ptr<Source<T>> source_;
  UpdateRequiredAware &update_ref_;
};

class FlowBase {
public:
  virtual ~FlowBase() = default;

  FlowBase(const FlowBase &) = delete;
  FlowBase &operator=(const FlowBase &) = delete;

  FlowBase(FlowBase &&) = delete;
  FlowBase &operator=(FlowBase &&) = delete;

  /**
   * @return the number of sources + nodes stored in this network
   */
  std::size_t size() const {
    std::scoped_lock guard(flow_lock_);
    return sources_.size() + nodes_.size();
  }
  /**
   * @return the number of sources stored in this network
   */
  std::size_t sources_size() const {
    std::scoped_lock guard(flow_lock_);
    return sources_.size();
  }
  /**
   * @return the number of nodes stored in this network
   */
  std::size_t nodes_size() const {
    std::scoped_lock guard(flow_lock_);
    return nodes_.size();
  }

protected:
  FlowBase() = default;

  mutable std::mutex flow_lock_;

  std::unordered_map<std::string, FlowElementPtr> labeled_elements_;

  std::vector<FlowElementPtr> sources_;
  std::unordered_map<FlowElement *, std::string> sources_to_labels_;

  std::vector<FlowElementResettablePtr> nodes_;
  std::unordered_map<FlowElementResettable *, std::string> nodes_to_labels_;
};

class UpdateRequiredAware : public virtual FlowBase {
  template <typename T> friend class HandlerSource;

protected:
  void propagateSourceUpdate(const FlowElement &source);

  std::unordered_set<FlowElementResettable *> updatePending_;
};

class HandlerFinder : public virtual UpdateRequiredAware {
public:
  /**
   * @return An handler wrapping an already generated source, if this one was
   * created with the passed label and has the specified type.
   */
  template <typename T>
  HandlerSource<T> findSource(const std::string &label) const {
    return find_<T, HandlerSource>(label);
  }

  /**
   * @return An handler wrapping an already generated node, if this one was
   * created with the passed label and has the specified type.
   */
  template <typename T> Handler<T> findNode(const std::string &label) const {
    return find_<T, Handler>(label);
  }

private:
  template <typename T, template <typename U> class HandlerU>
  HandlerU<T> find_(const std::string &label) const;
};

class HandlerMaker : public virtual UpdateRequiredAware {
public:
  template <typename T>
  HandlerSource<T> makeSource(T initial_value, const std::string &label = "");

  template <typename T, typename... As, typename Pred>
  Handler<T> makeNode(
      Pred &&lambda, const Handler<As> &...deps, const std::string &label = "",
      std::function<void(const T &)> valueCB = std::function<void(const T &)>{},
      std::function<void(const std::exception &)> errCB =
          std::function<void(const std::exception &)>{});

  template <typename T, typename... As, typename... ErrorsT, typename Pred>
  Handler<T> makeNodeWithErrorsCB(
      Pred &&lambda, const Handler<As> &...deps,
      ValueCallBacks<T, ErrorsT...> cb = ValueCallBacks<T, ErrorsT...>{},
      const std::string &label = "");

  enum class OnNewNodePolicy { IMMEDIATE_UPDATE, DEFERRED_UPDATE };
  auto onNewNodePolicy() const { return policy.load(); }
  void setOnNewNodePolicy(OnNewNodePolicy p) { policy.store(p); }

private:
  template <std::size_t Index, typename TupleT, typename Afront,
            typename... Arest>
  void packDeps(TupleT &recipient, const Handler<Afront> &dep_front,
                const Handler<Arest> &...dep_rest);

  std::atomic<OnNewNodePolicy> policy{OnNewNodePolicy::IMMEDIATE_UPDATE};
};

enum class FlowStatus { UPDATE_NOT_REQUIRED, UPDATE_REQUIRED, UPDATING };

class Updater : public virtual UpdateRequiredAware {
public:
  FlowStatus status() const {
    if (updating_.load(std::memory_order_acquire)) {
      return FlowStatus::UPDATING;
    }
    std::scoped_lock guard(flow_lock_);
    return updatePending_.empty() ? FlowStatus::UPDATE_NOT_REQUIRED
                                  : FlowStatus::UPDATE_REQUIRED;
  }

  /**
   * @brief triggers the update of all the nodes in the network.
   * Changes should happen as a consequence of having changed before any
   * source(s) value(s).
   */
  void update();

  /**
   * @brief sets the number of threads to use for the next network update, i.e.
   * when update() will be called again.
   */
  void setThreads(std::size_t threads) {
    if (threads == 0) {
      throw Error{"Specify at least one thread for the update"};
    }
    threads_.store(threads, std::memory_order_release);
  }

private:
  std::unordered_set<FlowElementResettable *>
  updateSingle(std::vector<FlowElementResettable *> open);

  std::unordered_set<FlowElementResettable *>
  updateMulti(std::unordered_set<FlowElementResettable *> open,
              std::size_t threads_to_use);

  std::atomic<std::size_t> threads_ = 1;
  std::atomic_bool updating_{false};
};

struct ValueSnapshot {
  bool has_value;
  std::size_t id;
  std::size_t generation;
  std::string label;
  std::string value_serialization;
  // the Ids of the node whose result depend on this one
  std::vector<std::size_t> dependants;
};
using FlowSnapshot = std::vector<ValueSnapshot>;

class StructureExporter : public virtual FlowBase {
public:
  /**
   * @return a serialization of the entire network
   */
  FlowSnapshot snapshot() const;
};

class Flow final : public HandlerFinder,
                   public HandlerMaker,
                   public Updater,
                   public StructureExporter {
public:
  Flow() = default;
};

} // namespace flw
