/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_VALUE_AWARE_H
#define FLOW_VALUE_AWARE_H

#include <components/ValueStorer.hpp>

namespace flw {

	template<typename T>
	class ValueAware {
	public:
		inline bool isValue() const { return this->storer->value.isValue(); };
		inline bool isException() const { return this->storer->value.isException(); };

		inline std::exception_ptr getException() const {
            this->storer->value.getException();
		};

        bool useValue(const std::function<void(const T&)>& action) const {
            std::lock_guard<std::mutex> lock(this->storer->value.valueMtx);
            if (isException()) {
                throw Error("This object is an exception and not a value");
            }
            if (!isValue()) {
                return false;
            }
            action(*this->storer->value.get());
            return true;
        }

	protected:
        ValueAware(const std::shared_ptr<ValueStorer>& storer)
            : storer(storer) {
        };

	private:
        std::shared_ptr<ValueStorer> storer;
	};
}

#endif