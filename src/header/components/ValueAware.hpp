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
        friend class ValueAwareExtractor;
	public:
        virtual ~ValueAware() = default;

		bool isValue() const {
            std::lock_guard<std::mutex> lock(this->storer->valueMtx);
            return this->storer->value.isValue(); 
        };
		bool isException() const {
            std::lock_guard<std::mutex> lock(this->storer->valueMtx);
            return this->storer->value.isException(); 
        };

		std::exception_ptr getException() const {
            std::lock_guard<std::mutex> lock(this->storer->valueMtx);
            return this->storer->value.getException();
		};

        template<typename FunctionT>
        bool useValue(FunctionT action) const {
            std::lock_guard<std::mutex> lock(this->storer->valueMtx);
            if (isException()) {
                throw Error("This object is an exception and not a value");
            }
            if (!isValue()) {
                return false;
            }
            action(*this->storer->value.get());
            return true;
        }

        inline std::size_t getGeneration() const {
            return this->storer->generations;
        }

	protected:
        ValueAware(const std::shared_ptr<ValueStorer<T>>& storer)
            : storer(storer) {
        };

        ValueAware(const ValueAware<T>& o) 
            : ValueAware(o.storer) {
        };
        ValueAware<T>& operator==(const ValueAware<T>& o) {
            storer = o.storer;
            return *this;
        }

        std::shared_ptr<ValueStorer<T>> storer;
	};
}

#endif
