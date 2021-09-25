/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_VALUE_STORER_H
#define FLOW_VALUE_STORER_H

#include <components/ValueOrException.hpp>
#include <mutex>

namespace flw {

    template<typename T>
    class ValueStorer {
    public:
        virtual ~ValueStorer() = default;

        mutable std::mutex valueMtx;
        ValueOrException<T> value;

    protected:
        ValueStorer() = default;
    };

}

#endif
