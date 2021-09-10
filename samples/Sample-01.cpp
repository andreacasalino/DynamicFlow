#include <components/ValueOrException.h>
#include <iostream>

class String {
public:
	String(const std::string& mex) {
		if (mex.empty()) {
			throw flw::Error("Empty message");
		}
		this->mex = mex;
	};

	inline const std::string& getMex() const { return this->mex; };

private:
	std::string mex;
};

std::ostream& operator<<(std::ostream& s, flw::ValueOrException<String>& value) {
	if (value.isException()) {
		s << "exception: ";
		auto exc = value.extractException();
		try {
			std::rethrow_exception(exc);
		}
		catch (const std::exception& e) {
			s << e.what();
		}
	}
	else if (nullptr == value) {
		s << "nullptr";
	}
	else {
		s << "Value: " << value.get().getMex();
	}
	return s;
}

int main() {
	flw::ValueOrException<String> value;
	std::cout << value << std::endl;

	value.reset("Ciao");
	std::cout << value << std::endl;

	value.reset("");
	std::cout << value << std::endl;
	std::cout << value << std::endl;

	return EXIT_SUCCESS;
}
