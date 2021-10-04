/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_FLOW_H
#define FLOW_FLOW_H

#include <flow/SourceHandler.hpp>
#include <flow/NodeHandler.hpp>
#include <map>
#include <set>
#include <chrono>

namespace flw {

    class Flow {
    public:
        Flow() = default;

        Flow(const Flow&) = delete;
        Flow& operator==(const Flow&) = delete;
        Flow(Flow&&) = delete;
        Flow& operator==(Flow&&) = delete;

        template<typename T>
        SourceHandler<T> makeSource(const std::string& name) {
            checkName(name);
            Source<T>* impl = new Source<T>(name);
            std::shared_ptr<Source<T>> source;
            source.reset(impl);
            sources.emplace(source->getName(), source);
            allTogether.emplace(source->getName(), source);
            return SourceHandler<T>(source);
        }

        template<typename T>
        SourceHandler<T> findSource(const std::string& name) {
            auto it = sources.find(name);
            if (it == sources.end()) {
                throw Error("Inexistent");
            }
            std::shared_ptr<Source<T>> impl = std::dynamic_pointer_cast<Source<T>, FlowEntity>(it->second);
            if (nullptr == impl) {
                throw Error("Wrong type asked");
            }
            return impl;
        };

        template<typename T, typename ... Ts, typename ... Args>
        NodeHandler<T, Ts...> makeNode(const std::string& name, const std::function<T(const Ts & ...)>& evaluation, const Args& ... handlers) {
            checkName(name);
            Node<T, Ts...>* impl = new Node<T, Ts...>(name, evaluation, handlers...);
            std::shared_ptr<Node<T, Ts...>> node;
            node.reset(impl);
            nodes.emplace(node->getName(), node);
            allTogether.emplace(node->getName(), node);
            return NodeHandler<T, Ts...>(node);
        }

        template<typename T, typename ... Ts>
        NodeHandler<T, Ts...> findNode(const std::string& name) {
            auto it = nodes.find(name);
            if (it == nodes.end()) {
                throw Error("Inexistent");
            }
            std::shared_ptr<Node<T, Ts...>> impl = std::dynamic_pointer_cast<Node<T, Ts...>, FlowEntity>(it->second);
            if (nullptr == impl) {
                throw Error("Wrong type asked");
            }
            return impl;
        };

        template<typename ... UpdateInputs>
        void updateFlow(UpdateInputs&& ... inputs) {
            std::lock_guard<std::mutex> updateLock(updateValuesMtx);
            busy = true;
            std::set<EvaluateCapable*> toUpdate;
            updateSource(toUpdate, std::forward<UpdateInputs>(inputs)...);
            toUpdate = computeUpdateRequired(toUpdate);
            updateNodes(toUpdate);
            busy = false;
        }

        void waitUpdateComplete(const std::chrono::microseconds& maxWaitTime = std::chrono::microseconds(0)) const;

        bool isBusy() const;

    private:
        void checkName(const std::string& name) {
            auto it = allTogether.find(name);
            if (it != allTogether.end()) {
                throw Error("Name already reserved");
            }
        }

        template<typename T, typename ... UpdateInputs>
        void updateSource(std::set<EvaluateCapable*>& toUpdate, 
                          const std::string& source_name, std::unique_ptr<T> new_value, 
                          UpdateInputs&& ... remaining) {
            this->template updateSource<T>(toUpdate, source_name, std::move(new_value));
            updateSource(toUpdate, std::forward<UpdateInputs>(remaining)...);
        }
        template<typename T>
        void updateSource(std::set<EvaluateCapable*>& toUpdate,
            const std::string& source_name, std::unique_ptr<T> new_value) {
            SourceHandler<T> handler = this->template findSource<T>(source_name);
            handler.reset(std::move(new_value));
            Source<T>* impl = dynamic_cast<Source<T>*>(handler.storer.get());
            for (auto* d : impl->descendants) {
                toUpdate.emplace(d);
            }
        }

        std::set<EvaluateCapable*> computeUpdateRequired(const std::set<EvaluateCapable*>& initialNodes);

        void updateNodes(std::set<EvaluateCapable*> toUpdate);

        std::map<FlowName, FlowEntityPtr> sources;
        std::map<FlowName, FlowEntityPtr> nodes;

        std::map<FlowName, FlowEntityPtr> allTogether;

        mutable std::mutex updateValuesMtx;
        std::atomic_bool busy = false;
    };
}

#endif
