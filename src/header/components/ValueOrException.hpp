/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#ifndef FLOW_VALUE_H
#define FLOW_VALUE_H

#include <Error.h>
#include <memory>

namespace flw {

	template<typename T>
	class ValueOrException {
	public:
		/** @brief nullptr c'tor
		 */
		ValueOrException() = default;

		ValueOrException(T* rawValue) {
			this->reset(rawValue);
		}

		template<typename ... Args>
		ValueOrException(Args ... args) {
			this->reset(args...);
		};

		ValueOrException(const std::exception_ptr& exception) {
			this->exception = exception;
		};

		void reset(T* newValue = nullptr) {
			this->value.reset(newValue);
		};

		template<typename ... Args>
		void reset(Args ... args) {
			this->value.reset();
			exception = std::exception_ptr();
			try {
				this->value = std::make_unique<T>(args...);
			}
			catch (const Error& e) {
				this->value.reset();
				this->exception = std::make_exception_ptr(e);
			}
			catch (const std::exception & e) {
				this->value.reset();
				this->exception = std::make_exception_ptr(e);
			}
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
