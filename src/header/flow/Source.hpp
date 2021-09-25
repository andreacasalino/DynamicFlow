/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_SOURCE_H
#define FLOW_SOURCE_H

#include <components/ValueStorer.hpp>
#include <components/DescendantsAware.hpp>

namespace flw {

    template<typename T>
    class Source
        : public ValueStorer<T>
        , public DescendantsAware {
    public:
        template<typename ... Args>
        void reset(Args ... args);

    protected:
        Source() = default;
    };

}

#endif
