#include <tuple>
#include <utility>
#include <string>
#include <iostream>

void function(const int& a, const float& b, const std::string& c) {
	std::cout << "hello from function" << std::endl;
}

int main() {
	std::tuple<int, float, std::string> data;

	std::apply([](auto&... ts) {(function(ts...)); }, data);

	return EXIT_SUCCESS;
}
