/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_ANCESTORS_AWARE_H
#define FLOW_ANCESTORS_AWARE_H

#include <components/ValueAware.hpp>
#include <components/TypeExtractor.hpp>

namespace flw {

    template<std::size_t Position, typename T>
    class AncestorAware {
    public:
        ValueAware<T>* ancestor = nullptr;
    };

    template<std::size_t Position, typename ... Ts>
    class AncestorsAwareRecurr {
    public:
        static const std::size_t AncestorsSize = Position;
    };

    template<std::size_t Position, typename T, typename ... Ts>
    class AncestorsAwareRecurr<Position, T, Ts...>
        : public AncestorAware<Position, T>
        , public AncestorsAwareRecurr<Position+1, Ts...> {
    };

    template<typename ... Ts>
    class AncestorsAware
        : public AncestorsAwareRecurr<0, Ts...> {
    public:
        template<typename ... Values>
        AncestorsAware(Values ... values) {
            //bind<0, Values...>(values...);
        };

        template<std::size_t Index>
        auto& getAncestorAware() {
            return static_cast<AncestorAware<Index, TypeExtractor<Index, Ts...>::Type >&>(*this);
        }

        template<std::size_t Index>
        const auto& getAncestorAware() const {
            return static_cast<const AncestorAware<Index, TypeExtractor<Index, Ts...>::Type >&>(*this);
        }

    private:
        template<std::size_t Index, typename ... Values>
        void bind(ValueAware<TypeExtractor<Index, Ts...>::Type>& value, Values ... values) {
            //bind<Position, Value>(value);
            //bind<Position +1, Values...>(values...);
        };

        template<std::size_t Position>
        void bind(ValueAware<TypeExtractor<Index, Ts...>::Type>& value) {
            //auto& ancestor = getAncestorAware<Position>();
            //ancestor.ancestor = &value;
        };
    };

}

#endif
