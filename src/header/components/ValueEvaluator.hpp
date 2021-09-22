/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_VALUE_EVALUATOR_H
#define FLOW_VALUE_EVALUATOR_H

#include <components/ValueAware.hpp>
#include <functional>
#include <list>

namespace flw {

    using T = int;
    class ValueEvaluator 
        : public ValueAware<T> {
    protected:
        template<typename EvaluationT, typename ... Predecessors>
        ValueEvaluator(EvaluationT evaluation, Predecessors ... pred);

        std::function<T(void)> evaluation;
        std::list<ValueAware<T>> predecessors; // here is wrong now cause each predecessor will have its own type
    };

}

#endif
