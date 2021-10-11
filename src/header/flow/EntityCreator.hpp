/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#pragma once

#include <flow/EntityAware.hpp>
#include <flow/NodeHandler.hpp>
#include <flow/Updater.h>
#include <set>

namespace flw {

class EntityCreator : virtual public EntityAware,
                      virtual public Updater,
                      public SourceMaker,
                      public NodeMaker,
                      public ValueAwareStorerExtractor {
public:
  template <typename T> SourceHandler<T> makeSource(const std::string &name) {
    std::lock_guard<std::mutex> creationLock(entityCreationMtx);
    checkName(name);
    Source<T> *impl = this->template makeSource_<T>(name);
    std::shared_ptr<Source<T>> source;
    source.reset(impl);
    sources.emplace(source->getName(), source);
    allTogether.emplace(source->getName(), source);
    return SourceHandler<T>(source);
  }

  template <typename T, typename... Ts, typename... Args>
  NodeHandler<T> makeNode(const std::string &name,
                          const std::function<T(const Ts &...)> &evaluation,
                          const Args &...handlers) {
    std::lock_guard<std::mutex> creationLock(entityCreationMtx);
    std::lock_guard<std::mutex> updaterLock(updateValuesMtx);
    checkName(name);
    checkIsInternalEntity(handlers...);
    Node<T, Ts...> *impl = this->template makeNode_<Node<T, Ts...>>(
        name, evaluation, extractStorer(handlers)...);
    std::shared_ptr<Node<T, Ts...>> node;
    node.reset(impl);
    nodes.emplace(node->getName(), node);
    allTogether.emplace(node->getName(), node);
    requiringUpdate.emplace(node.get());
    return NodeHandler<T>(node);
  }

protected:
  void checkName(const std::string &name) {
    auto it = allTogether.find(name);
    if (it != allTogether.end()) {
      throw Error(name, " is an already reserved name");
    }
  }

  template <typename EntityT, typename... Args>
  void checkIsInternalEntity(const EntityT &entity, const Args &...remaining) {
    checkIsInternalEntity(entity);
    checkIsInternalEntity(remaining...);
  }

  template <typename EntityT>
  void checkIsInternalEntity(const EntityT &entity) {
    const FlowEntity *entityPtr =
        dynamic_cast<const FlowEntity *>(&extractStorer(entity));
    if (nullptr == entityPtr) {
      throw Error("Not a valid entity");
    }
    auto it = allTogether.find(entityPtr->getName());
    if (it == allTogether.end()) {
      throw Error(*entityPtr->getName().get(), " is not a entity of this flow");
    }
    if (it->second.get() != entityPtr) {
      throw Error(*entityPtr->getName().get(), " is not a entity of this flow");
    }
  }
};

} // namespace flw
