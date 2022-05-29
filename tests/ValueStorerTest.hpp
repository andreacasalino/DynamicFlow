/**
 * Author:    Andrea Casalino
 * Created:   10.09.2021
 *
 * report any bug to andrecasa91@gmail.com.
 **/

#include <DynamicFlow/components/ValueStorer.hpp>

template <typename T> class ValueStorerTest : public flw::ValueStorer<T> {
public:
  ValueStorerTest() = default;

  template <typename... Args> void reset(Args... args) {
    try {
      this->value.reset(std::make_unique<T>(args...));
    } catch (const std::exception &e) {
      this->value.resetException(std::make_exception_ptr(e));
    }
  };
};
