/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_NODE_H
#define FLOW_NODE_H

#include <components/ValueEvaluator.hpp>
#include <components/DescendantsAware.hpp>

namespace flw {

    template<typename T>
    class Node
        : private ValueEvaluator<T>
        , public DescendantsAware {
    public:
        void useValue(const std::function<void(const ValueOrException<T>&)>& action);
    };

}

#endif
