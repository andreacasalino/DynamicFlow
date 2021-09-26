/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_NODE_H
#define FLOW_NODE_H

#include <flow/FlowEntity.h>
#include <components/ValueAware.hpp>
#include <components/DescendantsAware.hpp>
#include <components/Evaluator.hpp>

namespace flw {

    class ValueAwareExtractor {
    public:
        template<typename ValueAwareT>
        const auto* extract(const ValueAwareT& subject) const {
            return subject.storer.get();
        }
    };

    template<typename T, typename ... Ts>
    class Node
        : public FlowEntity
        , public DescendantsAware
        , public Evaluator<T, Ts...>
        , public ValueAwareExtractor {
        friend class Flow;
    protected:
        Node(const std::string& name, const std::function<T(const Ts & ...)>& evaluation) 
            : FlowEntity(name)
            , Evaluator<T, Ts...>(evaluation) {
        }

        template<typename ... Values>
        Node(const std::string& name, const std::function<T(const Ts & ...)>& evaluation, const Values& ... handlers)
            : Node(name, evaluation) {
            bindSubscribeHandlers<0, Values...>(handlers...);
        };

    protected:
        template<typename ... Values>
        void subscribe(const DescendantsAware& ancestor, const Values& ... ancestors) {
            subscribe(ancestor);
            subscribe(ancestors...);
        };

        void subscribe(const DescendantsAware& ancestor) {
            std::lock_guard<std::mutex> lock(ancestor.descendantsMtx);
            ancestor.descendants.push_back(this);
        };

        template<std::size_t Index, typename Value, typename ... Values>
        void bindSubscribeHandlers(const Value& handler, const Values& ... handlers) {
            bindSubscribeHandlers<Index, Value>(handler);
            bindSubscribeHandlers<Index + 1, Values...>(handlers...);
        }

        template<std::size_t Index, typename Value>
        void bindSubscribeHandlers(const Value& handler) {
            const auto* storer = extract(handler);
            bind<Index>(*storer);

            const DescendantsAware* asDescAware = dynamic_cast<const DescendantsAware*>(storer);
            subscribe(*asDescAware);
        };
    };

}

#endif
