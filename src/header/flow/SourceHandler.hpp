/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_SOURCE_HANDLER_H
#define FLOW_SOURCE_HANDLER_H

#include <components/ValueAware.hpp>
#include <flow/Source.hpp>

namespace flw {

    template<typename T>
    class SourceHandler
        : public ValueAware<T> {
    public:
        SourceHandler(std::shared_ptr<Source<T>> sourceImpl)
            : ValueAware<T>(sourceImpl) {
        }

        SourceHandler(const SourceHandler<T>& o) 
            : ValueAware<T>(0) {
        };
        SourceHandler<T>& operator==(const SourceHandler<T>& o) {
            static_cast<ValueAware<T>&>(*this) = static_cast<const ValueAware<T>&>(o);
        };

        template<typename ... Args>
        void reset(Args ... args) {
            Source<T>* sourcePt = dynamic_cast<Source<T>*>(storer.get());
            sourcePt->reset(args...);
        };
    };
}

#endif
