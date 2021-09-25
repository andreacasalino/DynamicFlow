/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_VALUE_OR_EXCEPTION_H
#define FLOW_VALUE_OR_EXCEPTION_H

#include <Error.h>
#include <memory>

namespace flw {

	template<typename T>
	class ValueOrException {
	public:
		/** @brief nullptr c'tor
		 */
		ValueOrException() = default;

		ValueOrException(std::unique_ptr<T> newValue) {
			this->reset(std::move(newValue));
		}

		void reset(std::unique_ptr<T> newValue = nullptr) {
			this->exception = std::exception_ptr();
			this->value = std::move(newValue);
		};

		ValueOrException(const std::exception_ptr& exception) {
			this->reset(exception);
		};

		void reset(const std::exception_ptr& exception) {
			this->exception = exception;
			this->value.reset();
		};

		inline bool isValue() const { return (nullptr != this->value); };
		inline bool isException() const { return (nullptr != this->exception); };

		inline std::exception_ptr getException() const {
			return this->exception;
		};

		[[nodiscard]] const T* get() const {
			return this->value.get();
		};

		[[nodiscard]] T* get() {
			return this->value.get();
		};

	private:
		std::unique_ptr<T> value;
		std::exception_ptr exception;
	};

	template<typename T>
	inline bool operator==(std::nullptr_t, const ValueOrException<T>& o) { return !o.isValue(); };
	template<typename T>
	inline bool operator==(const ValueOrException<T>& o, std::nullptr_t) { return !o.isValue(); };
}

#endif
