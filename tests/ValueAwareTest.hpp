/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <components/ValueAware.hpp>

template<typename T>
class ValueAwareTest
	: public flw::ValueAware<T> {
public:
	ValueAwareTest() = default;
};
