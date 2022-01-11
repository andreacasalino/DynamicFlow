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
                      public ValueStorerExtractor {
public:
  /**
   * @brief Creates a new source inside this flow
   * @input the name of the source
   * @return An handler storing the newly created source
   * @throw In case a source with the passed name already exists in this flow
   */
  template <typename T> SourceHandler<T> makeSource(const std::string &name) {
    std::scoped_lock<std::mutex> creationLock(entityCreationMtx);
    checkName(name);
    Source<T> *impl = this->template makeSource_<T>(name);
    std::shared_ptr<Source<T>> source;
    source.reset(impl);
    sources.emplace(source->getName(), source);
    allTogether.emplace(source->getName(), source);
    return SourceHandler<T>(source);
  }

  /**
   * @brief Creates a new node inside this flow
   * @input the name of the source
   * @input the expression that the node to create should use to internally
   * update its value
   * @input the entities that the node to create depends on
   * @return An handler storing the newly created node
   * @throw In case a node with the passed name already exists in this flow
   * @throw In case one of the passed handlers is neither a NodeHandler nor a
   * SourceHandler
   * @throw In case one of the passed handlers is not contained in this flow
   */
  template <typename T, typename... Ts, typename... Args>
  NodeHandler<T> makeNode(const std::string &name,
                          const std::function<T(const Ts &...)> &evaluation,
                          const Args &...handlers) {
    std::scoped_lock creationLock(entityCreationMtx, updateValuesMtx);
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

  template <typename FlowT> void absorb(FlowT &&o) {
    auto lockCreationOther = makeEntityCreationMtxLock(o);
    auto lockUpdateOther = makeUpdateValuesMtxLock(o);

    std::scoped_lock lockCreation(entityCreationMtx, updateValuesMtx);

    this->sources = std::move(o.sources);
    this->nodes = std::move(o.nodes);
    this->allTogether = std::move(o.allTogether);
    this->requiringUpdate = std::move(o.requiringUpdate);

    o.sources.clear();
    o.nodes.clear();
    o.allTogether.clear();
    o.requiringUpdate.clear();
  };

protected:
  void checkName(const std::string &name) {
    auto it = allTogether.find(FlowName{name});
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
    const auto *entityPtr =
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
