#include <iostream>
#include <memory>

template<std::size_t Pos, typename T>
class ValueAware {
protected:
	std::unique_ptr<T> value = std::make_unique<T>();
};

template<std::size_t Pos, typename T, typename ... Ts>
class ValueAwareList
	: public ValueAware<Pos, T>
#if (sizeof...(Ts) > 1)
	, public ValueAwareList<Pos + 1, Ts...>
#endif {
};

template<typename ... Ts>
class Storer
	: public ValueAwareList<0, Ts...> {
public:
	template<std::size_t PosT, typename T>
	auto* get() {
		return ValueAware<PosT, T>::value.get();
	}
};

int main() {
	Storer<int, float> storer;

	auto* p1 = storer.get<0, int>();
	*p1 = 0;
	std::cout << *p1 << std::endl;

	return EXIT_SUCCESS;
}
