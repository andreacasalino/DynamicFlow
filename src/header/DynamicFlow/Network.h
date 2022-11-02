/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <DynamicFlow/Node.hpp>

#include <list>
#include <unordered_map>

namespace flw {
namespace detail {
class HandlerMaker;
class HandlerFinder;

class FlowBase;
} // namespace detail

template <typename T> class NodeHandler : public detail::ValueAware<T> {
  friend class detail::HandlerMaker;
  friend class detail::HandlerFinder;

public:
  /**
   * @return the name used to register the node wrapped by this handler
   */
  const std::optional<std::string> &label() const { return label_; }

  /**
   * @brief forward to ValueTyped::onNewValueCallBack(...) of the value wrapped
   * by this handler.
   */
  template <typename Predicate> void onNewValueCallBack(Predicate &&cb) {
    this->value_->onNewValueCallBack(std::forward<Predicate>(cb));
  }

  /**
   * @brief forward to ValueTyped::onNewExceptionCallBack(...) of the value
   * wrapped by this handler.
   */
  template <typename Predicate> void onNewExceptionCallBack(Predicate &&cb) {
    this->value_->onNewExceptionCallBack(std::forward<Predicate>(cb));
  }

protected:
  NodeHandler(const std::optional<std::string> &label,
              const std::shared_ptr<ValueTyped<T>> &value)
      : detail::ValueAware<T>{value}, label_(label) {}

  NodeHandler(const std::optional<std::string> &label,
              const detail::ValueAware<T> &o)
      : detail::ValueAware<T>{o}, label_(label) {}

private:
  const std::optional<std::string> label_;
};

template <typename T> class SourceHandler : public NodeHandler<T> {
  friend class detail::HandlerMaker;
  friend class detail::HandlerFinder;

public:
  /**
   * @brief update the value stored by the wrapped source, using the passed
   * arguments.
   *
   * Attention!! This method actually updates only the wrapped source. In order
   * to trigger the update of all the dependant nodes, you need to do it from
   * the containing network by calling Updater::update().
   */
  template <typename... Args> void update(Args... args) {
    this->value_->update([&args...]() { return T{args...}; });
  }

protected:
  using NodeHandler<T>::NodeHandler;
};

enum class FlowStatus { IDLE, UPDATING };

namespace detail {
template <typename T>
class LabelsMap : public std::unordered_map<std::string, T> {
public:
  LabelsMap() = default;

  template <typename... Args>
  void emplaceWithCheck(const std::string &label, Args &&...args) {
    auto it = this->find(label);
    if (it != this->end()) {
      throw Error{"An element with label: ", label, " , already exists"};
    }
    this->emplace(label, std::forward<Args>(args)...);
  }
};

class FlowBase {
public:
  FlowBase(FlowBase &) = delete;
  FlowBase &operator=(FlowBase &) = delete;

  virtual ~FlowBase() = default;

  /**
   * @return the number of sources + nodes stored in this network
   */
  std::size_t size() const {
    std::scoped_lock lock(flow_mtx_);
    return sources_all_.size() + nodes_all_.size();
  }
  /**
   * @return the number of sources stored in this network
   */
  std::size_t sources_size() const {
    std::scoped_lock lock(flow_mtx_);
    return sources_all_.size();
  }
  /**
   * @return the number of nodes stored in this network
   */
  std::size_t nodes_size() const {
    std::scoped_lock lock(flow_mtx_);
    return nodes_all_.size();
  }

  /**
   * @return Steals all the sources and nodes of the passed network. Clearly,
   * all nodes and sources stop to exist in the passed network and are entirely
   * migrated to this one.
   */
  void absorb(FlowBase &o);

protected:
  FlowBase() = default;

  mutable std::mutex flow_mtx_;

  // sources
  std::list<std::shared_ptr<Value>> sources_all_;
  LabelsMap<std::shared_ptr<Value>> sources_labeled_;

  // nodes
  std::list<NodePtr> nodes_all_;
  LabelsMap<NodePtr> nodes_labeled_;

  // sources and nodes
  std::set<const Value *> values_all_;
};

class HandlerFinder : public virtual FlowBase {
public:
  /**
   * @return An handler wrapping an already generated source, if this one was
   * created with the passed label and has the specified type.
   */
  template <typename T>
  SourceHandler<T> findSource(const std::string &label) const {
    LabelsMap<std::shared_ptr<Value>>::const_iterator it;
    {
      std::lock_guard<std::mutex> lock(flow_mtx_);
      it = sources_labeled_.find(label);
      if (it == sources_labeled_.end()) {
        throw Error{"Not able to find source named: ", label};
      }
    }
    auto as_typed_source = std::dynamic_pointer_cast<ValueTyped<T>>(it->second);
    if (as_typed_source == nullptr) {
      throw Error{"Source named: ", label, " is not of the requested type"};
    }
    return SourceHandler<T>{label, as_typed_source};
  }

  /**
   * @return An handler wrapping an already generated node, if this one was
   * created with the passed label and has the specified type.
   */
  template <typename T>
  NodeHandler<T> findNode(const std::string &label) const {
    LabelsMap<NodePtr>::const_iterator it;
    {
      std::lock_guard<std::mutex> lock(flow_mtx_);
      it = nodes_labeled_.find(label);
      if (it == nodes_labeled_.end()) {
        throw Error{"Not able to find node named: ", label};
      }
    }
    auto as_typed_node =
        std::dynamic_pointer_cast<detail::ValueAware<T>>(it->second);
    if (as_typed_node == nullptr) {
      throw Error{"Node named: ", label, " is not of the requested type"};
    }
    return NodeHandler<T>{label, *as_typed_node};
  }
};

class HandlerMaker : public virtual FlowBase {
public:
  template <typename T>
  SourceHandler<T>
  makeSource(const std::optional<std::string> &label = std::nullopt) {
    using SourceT = ValueTypedWithErrors<T>;
    std::shared_ptr<SourceT> source = std::make_shared<SourceT>();
    {
      std::lock_guard<std::mutex> lock(flow_mtx_);
      if (label.has_value()) {
        sources_labeled_.emplaceWithCheck(label.value(), source);
      }
      sources_all_.push_back(source);
      values_all_.emplace(source.get());
    }
    return SourceHandler<T>{label, source};
  }

  template <typename T, typename... Deps, typename EvalPredicate>
  NodeHandler<T> makeNode(EvalPredicate &&predicate,
                          const std::optional<std::string> &label,
                          const NodeHandler<Deps> &...deps) {
    auto val = std::make_unique<ValueTypedWithErrors<T>>();
    return this->makeNodeWithMonitoredException<T, Deps...>(
        std::forward<EvalPredicate>(predicate), std::move(val), label, deps...);
  }

  template <typename T, typename... Deps, typename EvalPredicate>
  NodeHandler<T> makeNodeWithMonitoredException(
      EvalPredicate &&predicate,
      std::unique_ptr<ValueTyped<T>> value_with_monitored_exceptions,
      const std::optional<std::string> &label,
      const NodeHandler<Deps> &...deps) {
    using NodeT = Node<T, Deps...>;
    std::shared_ptr<ValueTyped<T>> value_catched;
    value_catched.reset(value_with_monitored_exceptions.release());
    std::shared_ptr<NodeT> node = std::make_shared<NodeT>(
        std::forward<EvalPredicate>(predicate), value_catched);
    {
      std::lock_guard<std::mutex> lock(flow_mtx_);
      this->bind<0, NodeT, Deps...>(*node, deps...);
      if (label.has_value()) {
        nodes_labeled_.emplaceWithCheck(label.value(), node);
      }
      nodes_all_.push_back(node);
      values_all_.emplace(value_catched.get());
    }
    return NodeHandler<T>{
        label,
        *node,
    };
  }

private:
  template <std::size_t N, typename NodeT, typename Dep, typename... Deps>
  void bind(NodeT &subject, const NodeHandler<Dep> &first,
            const NodeHandler<Deps> &...others) {
    this->bind<N, NodeT, Dep>(subject, first);
    this->bind<N + 1, NodeT, Deps...>(subject, others...);
  }

  template <std::size_t N, typename NodeT, typename Dep>
  void bind(NodeT &subject, const NodeHandler<Dep> &dep) {
    if (this->values_all_.find(&dep.getValue()) == this->values_all_.end()) {
      throw Error{"Found invalid dependency when generating a new node"};
    }
    subject.template bind<N, Dep>(dep.getValue());
  }
};

class Updater : public virtual FlowBase {
public:
  FlowStatus status() const { return status_.load(); }

  /**
   * @brief triggers the update of all the nodes in the network.
   * Changes should happen as a consequence of having changed before nay
   * source(s) value(s).
   */
  void update();

  /**
   * @return true in case any node in the network needs an update, i.e. calling
   * update() would actually change at least one node in the network.
   */
  bool isUpdateRequired() const;

  /**
   * @brief sets the number of threads to use for the next network update, i.e.
   * when update() will be called again.
   */
  void setThreads(const std::size_t threads);

  /**
   * @brief resets all sources/nodes in the network, leaving them all in an
   * unset state.
   */
  void reset();

private:
  mutable std::mutex update_mtx_;
  std::atomic<FlowStatus> status_ = FlowStatus::IDLE;
  std::atomic<std::size_t> threads_ = 1;
};
} // namespace detail

struct ValueSnapshot {
  std::size_t id;
  std::optional<std::string> label;
  std::size_t epoch;
  ValueStatus status;
  std::string value_serialization;
  std::optional<std::vector<std::size_t>>
      dependencies; // actually, the id(s) of the dependencies
};
using FlowSnapshot = std::vector<ValueSnapshot>;

namespace detail {
class StructureExporter : public virtual FlowBase {
public:
  /**
   * @return a serialization of the entire network
   */
  FlowSnapshot getSnapshot(bool serializeNodeValues = false) const;
};
} // namespace detail

class Flow final : public detail::HandlerFinder,
                   public detail::HandlerMaker,
                   public detail::Updater,
                   public detail::StructureExporter {
public:
  Flow() = default;

  Flow(Flow &&o) { this->absorb(o); };
  Flow &operator=(Flow &&) = delete;
};
} // namespace flw
