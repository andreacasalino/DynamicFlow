#include <DynamicFlow/Network.h>

#include <algorithm>
#include <map>
#include <omp.h>

namespace flw {

namespace detail {
namespace {
template <typename T>
void absorb_map_(std::unordered_map<std::string, T> &recipient,
                 std::unordered_map<std::string, T> &giver) {
  auto giver_it = giver.begin();
  while (giver_it != giver.end()) {
    recipient.emplace(giver_it->first, std::move(giver_it->second));
    giver_it = giver.erase(giver_it);
  }
}

template <typename T>
void absorb_list_(std::list<T> &recipient, std::list<T> &giver) {
  recipient.insert(recipient.end(), giver.begin(), giver.end());
  giver.clear();
}

template <typename T>
void absorb_set_(std::set<T> &recipient, std::set<T> &giver) {
  recipient.insert(giver.begin(), giver.end());
  giver.clear();
}
} // namespace

void FlowBase::absorb(FlowBase &o) {
  std::scoped_lock lock(flow_mtx_, o.flow_mtx_);
  // check absorb is possible
  for (const auto &[name, source] : o.sources_labeled_) {
    if (this->sources_labeled_.find(name) != this->sources_labeled_.end()) {
      throw Error{"absorb failed: a source named: ", name, " already exists"};
    }
  }
  for (const auto &[name, node] : o.nodes_labeled_) {
    if (this->nodes_labeled_.find(name) != this->nodes_labeled_.end()) {
      throw Error{"absorb failed: a node named: ", name, " already exists"};
    }
  }
  // ok proceed
  absorb_map_(this->sources_labeled_, o.sources_labeled_);
  absorb_map_(this->nodes_labeled_, o.nodes_labeled_);
  absorb_list_(this->sources_all_, o.sources_all_);
  absorb_list_(this->nodes_all_, o.nodes_all_);
  absorb_set_(this->values_all_, o.values_all_);
}

void Updater::reset() {
  std::scoped_lock lock(flow_mtx_, update_mtx_);
  for (auto &source : sources_all_) {
    source->reset();
  }
  for (auto &node : nodes_all_) {
    node->resetValue();
  }
}

void Updater::setThreads(const std::size_t threads) {
  if (threads == 0) {
    throw Error{"Number of threads should be at least 1"};
  }
  threads_.store(threads);
}

void Updater::update() {
  std::lock_guard<std::mutex> update_lock(update_mtx_);
  status_.store(FlowStatus::UPDATING);
  const std::size_t threads_to_use = threads_.load();

  std::list<NodeBase *> open;
  Values closed;
  {
    std::lock_guard<std::mutex> lock(flow_mtx_);
    for (const auto &node : nodes_all_) {
      open.push_back(node.get());
    }
    for (const auto &source : sources_all_) {
      closed.emplace(source.get());
    }
  }

  struct ToClose {
    NodeStatus kind;
    std::list<NodeBase *>::iterator element;
  };
  std::vector<ToClose> to_close;
  auto close_parallel_for = [&to_close](int th_id, int threads) {
    for (int k = th_id; k < to_close.size(); k += threads) {
      auto &element = *to_close[k].element;
      switch (to_close[k].kind) {
      case NodeStatus::UPDATE_NOT_POSSIBLE:
        element->resetValue();
        break;
      case NodeStatus::UPDATE_POSSIBLE:
        element->update();
        break;
      case NodeStatus::UPDATE_NOT_REQUIRED:
        break;
      }
    }
  };

  auto any_deps_open = [&](const flw::NodeBase &subject) {
    for (const auto *dep : subject.dependencies()) {
      if (closed.find(dep) == closed.end()) {
        return true;
      }
    }
    return false;
  };

#pragma omp parallel num_threads(threads_to_use)
  {
    const auto th_id = omp_get_thread_num();
    const auto omp_threads = omp_get_num_threads();
    if (0 == th_id) {
      while (!open.empty()) {
        to_close.clear();
        for (auto open_it = open.begin(); open_it != open.end(); ++open_it) {
          auto *candidate = *open_it;
          if (any_deps_open(*candidate)) {
            continue;
          }
          const auto status = candidate->status();
          to_close.push_back(ToClose{status, open_it});
        }
#pragma omp barrier
        close_parallel_for(th_id, omp_threads);
#pragma omp barrier
        for (const auto &element : to_close) {
          closed.emplace((*element.element)->valueUntyped());
          open.erase(element.element);
        }
      }
      to_close.clear();
#pragma omp barrier
    } else {
      while (true) {
#pragma omp barrier
        if (to_close.empty()) {
          break;
        }
        close_parallel_for(th_id, omp_threads);
#pragma omp barrier
      }
    }
  }
  status_.store(FlowStatus::IDLE);
}

bool Updater::isUpdateRequired() const {
  std::scoped_lock lock(flow_mtx_, update_mtx_);
  for (const auto &node : nodes_all_) {
    switch (node->status()) {
    case NodeStatus::UPDATE_NOT_POSSIBLE:
    case NodeStatus::UPDATE_POSSIBLE:
      return true;
    }
  }
  return false;
}

FlowSnapshot StructureExporter::getSnapshot(bool serializeNodeValues) const {
  struct Info {
    std::size_t id;
    std::optional<std::string> label;
    std::optional<Values> dependencies;
  };

  std::map<const Value *, Info> meta_map;
  std::lock_guard<std::mutex> lock(flow_mtx_);
  std::size_t id = 0;
  for (const auto &[name, val] : sources_labeled_) {
    meta_map.emplace(val.get(), Info{id++, name});
  }
  for (const auto &val : sources_all_) {
    if (meta_map.find(val.get()) == meta_map.end()) {
      meta_map.emplace(val.get(), Info{id++});
    }
  }
  for (const auto &[name, node] : nodes_labeled_) {
    meta_map.emplace(node->valueUntyped(),
                     Info{id++, name, node->dependencies()});
  }
  for (const auto &node : nodes_all_) {
    auto *val = node->valueUntyped();
    if (meta_map.find(val) == meta_map.end()) {
      meta_map.emplace(val, Info{id++, std::nullopt, node->dependencies()});
    }
  }

  FlowSnapshot result;
  result.reserve(meta_map.size());
  for (const auto &[val, info] : meta_map) {
    auto &added = result.emplace_back();
    added.id = info.id;
    added.label = std::move(info.label);
    added.epoch = val->epoch();
    added.status = val->status();
    added.value_serialization = serializeNodeValues ? val->toString() : "none";

    if (info.dependencies) {
      auto &deps = added.dependencies.emplace();
      for (const auto *dep : info.dependencies.value()) {
        deps.push_back(meta_map.find(dep)->second.id);
      }
    }
  }
  return result;
}
} // namespace detail
} // namespace flw
