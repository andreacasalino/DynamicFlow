#include <DynamicFlow/Network.h>

#include <algorithm>
#include <map>
#include <omp.h>
#include <ranges>

namespace flw {
FlowSnapshot StructureExporter::snapshot() const {
  FlowSnapshot res;
  std::unordered_map<const FlowElement *, std::size_t> mapping;

  {
    std::scoped_lock guard(flow_lock_);

    auto forEachNode = [&](const auto &collection) {
      for (const auto &element : collection) {
        std::size_t id = res.size();
        std::string label;
        if constexpr (std::is_same_v<const FlowElementPtr &,
                                     decltype(element)>) {
          if (auto it = sources_to_labels_.find(element.get());
              it != sources_to_labels_.end()) {
            label = it->second;
          }
        } else {
          if (auto it = nodes_to_labels_.find(element.get());
              it != nodes_to_labels_.end()) {
            label = it->second;
          }
        }
        auto &&[generation, serialization] = element->serializationInfo();
        bool has_val = serialization != "null";
        res.emplace_back(ValueSnapshot{
            has_val,
            id,
            generation,
            std::move(label),
            std::move(serialization),
        });
        mapping.emplace(element.get(), id);
      }
    };
    forEachNode(sources_);
    forEachNode(nodes_);
  }

  auto link = [&](ValueSnapshot &recipient, const FlowElement &el) {
    for (const auto &dep : el.dependants()) {
      std::size_t dep_id = mapping.at(dep.get());
      recipient.dependants.push_back(dep_id);
    }
  };

  std::size_t index = 0;
  for (const auto &src : sources_) {
    link(res[index++], *src);
  }
  for (const auto &src : nodes_) {
    link(res[index++], *src);
  }

  return res;
}

void UpdateRequiredAware::propagateSourceUpdate(const FlowElement &source) {
  std::unordered_set<FlowElementResettable *> level, next;
  for (const auto &f : source.dependants()) {
    level.emplace(f.get());
  }
  // BFS to extend the update pending set
  while (!level.empty()) {
    updatePending_.insert(level.begin(), level.end());
    for (auto *n : level) {
      n->reset();
    }
    next.clear();
    for (auto *prev : level) {
      for (const auto &f : prev->dependants()) {
        if (updatePending_.find(f.get()) == updatePending_.end()) {
          next.emplace(f.get());
        }
      }
    }
    level = std::move(next);
  }
}

std::unordered_set<FlowElementResettable *>
Updater::updateSingle(std::vector<FlowElementResettable *> open) {
  std::vector<FlowElementResettable *> next;
  while (!open.empty()) {
    next.clear();
    for (auto *n : open) {
      if (n->updatePossible()) {
        n->update();
      } else {
        next.push_back(n);
      }
    }
    if (next.size() == open.size()) {
      break;
    }
    std::swap(open, next);
  }
  return std::unordered_set<FlowElementResettable *>{open.begin(), open.end()};
}

std::unordered_set<FlowElementResettable *>
Updater::updateMulti(std::unordered_set<FlowElementResettable *> open_set,
                     std::size_t threads_to_use) {
  std::vector<FlowElementResettable *> ready_to_process;
  std::atomic_bool life{true};
#pragma omp parallel num_threads(static_cast <int>(threads_to_use))
  {
    auto th_id = static_cast<std::size_t>(omp_get_thread_num());
    auto process = [&]() {
      for (std::size_t k = th_id; k < ready_to_process.size(); k += threads_to_use) {
        ready_to_process[k]->update();
      }
    };

    if(th_id == 0) {
      while(true) {
        std::size_t open_size_snap = open_set.size();
        auto ready_rng = open_set | std::views::filter([&open_set](FlowElementResettable* node) {
          const auto &deps = node->dependencies();
          return std::all_of(
            deps.begin(), deps.end(), [&open_set](const FlowElementResettablePtr& d) {
              return !open_set.contains(d.get());
            });
        });
        ready_to_process = {ready_rng.begin(), ready_rng.end()};
        for(auto* node : ready_to_process) {
          open_set.erase(node);
        }
        if(open_set.size() == open_size_snap || open_set.size() == open_size_snap) {
          life.store(false, std::memory_order_acquire);
          break;
        }
#pragma omp barrier
        process();
#pragma omp barrier
      }
#pragma omp barrier
    }
    else {
      while(true) {
#pragma omp barrier
        if(!life.load(std::memory_order_acquire)) {
          break;          
        }
        process();
#pragma omp barrier
      }
    }
  }

  return std::move(open_set);
}

void Updater::update() {
  std::scoped_lock guard(flow_lock_);
  if (updatePending_.empty()) {
    return;
  }

  updating_.store(true, std::memory_order_acquire);
  std::size_t threads_to_use = threads_.load(std::memory_order_acquire);
  if (threads_to_use == 1) {
    updatePending_ = updateSingle(std::vector<FlowElementResettable *>{
        updatePending_.begin(), updatePending_.end()});
  } else {
    updatePending_ = updateMulti(std::move(updatePending_), threads_to_use);
  }
  updating_.store(false, std::memory_order_release);
}
} // namespace flw
