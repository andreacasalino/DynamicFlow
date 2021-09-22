#include <string>
#include <iostream>
#include <memory>
#include <functional>

template<typename T>
class ValueAware {
public:
	ValueAware() = default;

	void set(const std::shared_ptr<T>& copy) {
		value = copy;
	};

	const T& get() const {
		return *value.get();
	}

protected:
	std::shared_ptr<T> value;
};


template<std::size_t Position, typename T>
class TupleNode
	: public ValueAware<T> {
public:
	ValueAware<T>* get() { return static_cast<ValueAware<T>*>(this); };
	const ValueAware<T>* get() const { return static_cast<const ValueAware<T>*>(this); };
};

template<std::size_t Position, typename ... Ts>
class TupleNodeRecurr {
public:
	const std::size_t TuplaSize = Position;
};

template<std::size_t Position, typename T, typename ... Ts>
class TupleNodeRecurr<Position, T, Ts...>
	: public TupleNode<Position, T>
	, public TupleNodeRecurr<Position + 1, Ts...> {
};


template<std::size_t Position, typename T, typename ... Ts>
struct TupleTypeSelector {
	using Type = typename TupleTypeSelector<Position-1, Ts...>::Type;
};

template<typename T, typename ... Ts>
struct TupleTypeSelector<0, T, Ts...> {
	using Type = T;
};


//template<typename TuplaT, typename ResT, std::size_t Index, typename ... PackedArgs>
//struct Evaluator {
//	static void eval(const TuplaT& tupla, std::shared_ptr<ResT>& result, const std::function<void(ResT&, Ts...)>& action, PackedArgs ... args) {
//		Evaluator<ResT, Index-1, PackedArgs...>::eval(tupla, result, action, tupla::get<Index>()->get(), args...);
//	};
//};
//
//template<typename TuplaT, typename ResT, typename ... PackedArgs>
//struct Evaluator<TuplaT, ResT, 0, PackedArgs...> {
//	static void eval(const TuplaT& tupla, std::shared_ptr<ResT>& result, const std::function<void(ResT&, Ts...)>& action, PackedArgs ... args) {
//		action(*result, tupla::get<0>()->get(), args...);
//	};
//};


template<typename ... Ts>
class Tuple
	: public TupleNodeRecurr<0, Ts...> {
public:
	template<std::size_t Index>
	auto* get() {
		return static_cast<TupleNode<Index, TupleTypeSelector<Index, Ts...>::Type>&>(*this).get();
	};

	template<std::size_t Index>
	auto* get() const {
		return static_cast<const TupleNode<Index, TupleTypeSelector<Index, Ts...>::Type>&>(*this).get();
	};

	//template<typename ResT>
	//void eval(std::shared_ptr<ResT>& result, const std::function<void(ResT & , Ts...)>& action) {
	//	Evaluator<Tuple<Ts...>, ResT, LastPosition>::eval(*this, result, action);
	//};
};


int main() {
	Tuple<int, float, int> tupla;

	std::shared_ptr<int> val1 = std::make_shared<int>(5);
	tupla.get<0>()->set(val1);
	std::shared_ptr<float> val2 = std::make_shared<float>(-3.f);
	tupla.get<1>()->set(val2);
	std::shared_ptr<int> val3 = std::make_shared<int>(10);
	tupla.get<2>()->set(val3);

	std::cout << tupla.TuplaSize << std::endl;

	return EXIT_SUCCESS;
}
