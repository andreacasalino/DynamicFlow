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
            auto it = sources.find(std::make_shared<const std::string>(name));
            if (it == sources.end()) {
                throw Error("Inexistent");
            }
            std::shared_ptr<Source<T>> impl = std::dynamic_pointer_cast<Source<T>, FlowEntity>(it->second);
            if (nullptr == impl) {
                throw Error("Wrong type asked");
            }
            return impl;
        };

    private:
        void checkName(const std::string& name) {
            auto it = allTogether.find(std::make_shared<const std::string>(name));
            if (it != allTogether.end()) {
                throw Error("Name already reserved");
            }
        }

        std::map<FlowName, FlowEntityPtr> sources;
        std::map<FlowName, FlowEntityPtr> nodes;

        std::map<FlowName, FlowEntityPtr> allTogether;
    };

}

#endif
