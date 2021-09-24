/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_VALUE_USER_H
#define FLOW_VALUE_USER_H

#include <components/ValueAware.hpp>

namespace flw {

    template<typename T>
    class ValueUser
        : public ValueAware<T> {
    public:
        ValueOrException<bool> useValue(const std::function<void(const T&)>& action) const {
            std::lock_guard<std::mutex> lock(valueMtx);
            if (value.isException()) {
                return ValueOrException<bool>(value.getException());
            }
            if (!value.isValue()) {
                return ValueOrException<bool>(false);
            }
            try {
                action(*value.get());
            }
            catch (const std::exception & e) {
                return ValueOrException<bool>(std::make_exception_ptr(e));
            }
            return ValueOrException<bool>(true);
        }
    };

}

#endif
