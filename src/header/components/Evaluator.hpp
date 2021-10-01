/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_EVALUATOR_H
#define FLOW_EVALUATOR_H

#include <components/AncestorsAware.hpp>
#include <components/EvaluateCapable.h>
#include <functional>

namespace flw {

    template<std::size_t Position, typename FunctionT, typename EvaluatorT, typename ResultT, typename T, typename ... Ts>
    struct EvaluatorRecurr {
        template<typename ... Args>
        static EvaluationResult evaluate(const FunctionT& evaluation, EvaluatorT& subject, const Args& ... args) {
            const auto& ancestor = subject.template getAncestor<Position>().ancestor;
            std::lock_guard<std::mutex> lock(ancestor->valueMtx);
            if (ancestor->value.isException()) {
                return EvaluationResult::BLOCKING_EXCEPTION;
            }
            if (!ancestor->value.isValue()) {
                return EvaluationResult::NOT_READY;
            }
            return EvaluatorRecurr<Position + 1, FunctionT, EvaluatorT, ResultT, Ts...>::evaluate(evaluation, subject, args..., *ancestor->value.get());
        }
    };

    template<std::size_t Position, typename FunctionT, typename EvaluatorT, typename ResultT, typename T>
    struct EvaluatorRecurr<Position, FunctionT, EvaluatorT, ResultT, T> {
        template<typename ... Args>
        static EvaluationResult evaluate(const FunctionT& evaluation, EvaluatorT& subject, const Args& ... args) {
            const auto& ancestor = subject.template getAncestor<Position>().ancestor;
            std::lock_guard<std::mutex> lock(ancestor->valueMtx);
            if (ancestor->value.isException()) {
                return EvaluationResult::BLOCKING_EXCEPTION;
            }
            if (!ancestor->value.isValue()) {
                return EvaluationResult::NOT_READY;
            }

            try {
                subject.value.reset( std::make_unique<ResultT>(evaluation(args..., *ancestor->value.get())) );
            }
            catch (const std::exception& e) {
                subject.value.reset(std::make_exception_ptr(e));
            }

            return EvaluationResult::SUCCESS;
        }
    };

    template<typename ResultT, typename ... Ts>
    class Evaluator
        : public ValueStorer<ResultT>
        , public AncestorsAware<Ts...>
        , public EvaluateCapable {
    public:
        EvaluationResult evaluate() override {
            std::lock_guard<std::mutex> lock(valueMtx);
            value.reset();
            return EvaluatorRecurr <0, std::function<ResultT(const Ts & ...)>, Evaluator<ResultT, Ts...>, ResultT, Ts...>::evaluate(evaluation, *this);
        }

    protected:
        template<typename ... Values>
        Evaluator(const std::function<ResultT(const Ts & ...)>& evaluation)
            : evaluation(evaluation) {
        };

    private:
        const std::function<ResultT(const Ts & ...)> evaluation;
    };

}

#endif
