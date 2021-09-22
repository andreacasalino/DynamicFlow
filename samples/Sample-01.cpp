#include <components/AncestorsAware.hpp>
using namespace flw;

template<typename T>
class ValueAwareTest
	: public flw::ValueAware<T> {
public:
	ValueAwareTest() = default;
};

int main() {
	ValueAwareTest<int> val0;
	ValueAwareTest<float> val1;
	ValueAwareTest<int> val2;

	AncestorsAware<int, float, int> sample(val0, val1, val2);

	return EXIT_SUCCESS;
}
