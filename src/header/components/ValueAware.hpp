/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_VALUE_AWARE_H
#define FLOW_VALUE_AWARE_H

#include <components/ValueOrException.hpp>
#include <mutex>

namespace flw {

    template<typename T>
    class ValueAware {
    public:
        mutable std::mutex valueMtx;
        ValueOrException<T> value;

    protected:
        ValueAware() = default;
    };

}

#endif
