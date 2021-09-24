/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_EVALUATOR_H
#define FLOW_EVALUATOR_H

#include <AncestorsAware.hpp>
#include <components/EvaluateCapable.h>
#include <functional>

namespace flw {

    template<std::size_t Position, typename FunctionT, typename EvaluatorT, typename T, typename ... Ts>
    struct EvaluatorRecurr {
        template<typename ... Args>
        static EvaluationResult evaluate(const FunctionT& evaluation, EvaluatorT& subject, const Args& ... args) {
            const auto& ancestor = subject.getAncestor<Position>().ancestor;
            std::lock_guard<std::mutex> lock(ancestor->valueMtx);
            if (ancestor->value.isException()) {
                return EvaluationResult::EXCEPTION;
            }
            if (!ancestor->value.isValue()) {
                return EvaluationResult::NOT_READY;
            }
            return EvaluatorRecurr<Position + 1, FunctionT, EvaluatorT, Ts...>::evaluate(evaluation, subject, args..., *ancestor->value.get());
        }
    };

    template<std::size_t Position, typename FunctionT, typename EvaluatorT, typename T>
    struct EvaluatorRecurr<Position, FunctionT, EvaluatorT, T> {
        template<typename ... Args>
        static EvaluationResult evaluate(const FunctionT& evaluation, EvaluatorT& subject, const Args& ... args) {
            const auto& ancestor = subject.getAncestor<Position>().ancestor;
            std::lock_guard<std::mutex> lock(ancestor->valueMtx);
            if (ancestor->value.isException()) {
                return EvaluationResult::EXCEPTION;
            }
            if (!ancestor->value.isValue()) {
                return EvaluationResult::NOT_READY;
            }
            evaluation(args..., *ancestor->value.get());
            return EvaluationResult::SUCCESS;
        }
    };

    template<typename T, typename ... Ts>
    class Evaluator
        : virtual public ValueAware<T>
        , public AncestorsAware<Ts...>
        , public EvaluateCapable {
    public:
        EvaluationResult evaluate() override {
            return EvaluatorRecurr <0, std::function<void(const Ts & ...)>, Evaluator<T, Ts...>, Ts...>::evaluate(evaluation, *this);
        }

    protected:
        template<typename ... Values>
        Evaluator(const std::function<void(const Ts & ...)>& evaluation)
            : evaluation(evaluation) {
        };

    private:
        const std::function<void(const Ts & ...)> evaluation;
    };

}

#endif
