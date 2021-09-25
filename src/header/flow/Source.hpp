/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_SOURCE_H
#define FLOW_SOURCE_H

#include <flow/FlowEntity.h>
#include <components/ValueStorer.hpp>
#include <components/DescendantsAware.hpp>

namespace flw {

    template<typename T>
    class Source
        : public FlowEntity
        , public ValueStorer<T>
        , public DescendantsAware {
        friend class Flow;
    public:
        template<typename ... Args>
        void reset(Args ... args);

    protected:
        Source(const std::string& name)
            : FlowEntity(name) {
        };
    };

}

#endif
