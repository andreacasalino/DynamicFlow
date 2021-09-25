/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_NODE_H
#define FLOW_NODE_H

#include <components/ValueStorer.hpp>
#include <components/DescendantsAware.hpp>
#include <components/Evaluator.hpp>

namespace flw {

    template<typename T, typename ... Ts>
    class Node
        : public DescendantsAware
        , public Evaluator<T, Ts...> {
    protected:
        template<typename ... Values>
        Node(const std::function<T(const Ts & ...)>& evaluation, const Values& ... ancestors) 
            : Evaluator<T, Ts...>(evaluation) {
            bind(ancestors...);
            subscribe(ancestors...);
        };

    private:
        template<typename ... Values>
        void subscribe(const DescendantsAware& ancestor, const Values& ... ancestors) {
            subscribe(ancestor);
            subscribe(ancestors...);
        };

        void subscribe(const DescendantsAware& ancestor) {
            std::lock_guard<std::mutex> lock(ancestor.descendantsMtx);
            ancestor.descendants.push_back(this);
        };
    };

}

#endif
