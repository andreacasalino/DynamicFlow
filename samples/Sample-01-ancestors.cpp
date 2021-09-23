#include <components/AncestorsAware.hpp>
#include <iostream>
using namespace flw;

template<typename T>
class ValueAwareTest
	: public flw::ValueAware<T> {
public:
	ValueAwareTest() = default;
};

template<std::size_t Position, typename AncestorsT, typename T, typename ... Ts>
struct AncestorsPrinter {
	static void print(std::ostream& s, const AncestorsT& subject) {
		const auto& ancestor = subject.getAncestor<Position>().ancestor;
		s << *ancestor->value.get() << " ; ";
		AncestorsPrinter<Position + 1, AncestorsT, Ts...>::print(s, subject);
	};
};

template<std::size_t Position, typename AncestorsT, typename T>
struct AncestorsPrinter<Position, AncestorsT, T> {
	static void print(std::ostream& s, const AncestorsT& subject) {
		const auto& ancestor = subject.getAncestor<Position>().ancestor;
		s << *ancestor->value.get();
	};
};

template<typename ... Ts>
std::ostream& operator<<(std::ostream& s, const AncestorsAware<Ts...>& subject) {
	AncestorsPrinter<0, AncestorsAware<Ts...>, Ts...>::print(s, subject);
	return s;
};

int main() {
	ValueAwareTest<int> val0;
	val0.value.reset(5);
	ValueAwareTest<float> val1;
	val1.value.reset(3.5f);
	ValueAwareTest<int> val2;
	val2.value.reset(-5);

	AncestorsAware<int, float, int> sample(val0, val1, val2);

	std::cout << sample << std::endl;

	return EXIT_SUCCESS;
}
