/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_SOURCE_H
#define FLOW_SOURCE_H

#include <components/ValueAware.hpp>
#include <components/DescendantsAware.hpp>

namespace flw {

    template<typename T>
    class Source
        : private ValueAware<T>
        , public DescendantsAware {
    public:
        Source();

        void update(std::unique_ptr<T> newValue);
    };

}

#endif
