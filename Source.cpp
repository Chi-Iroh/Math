#include "Matrix.hpp"
#include <iostream>
#include <string>
#include "Fraction.hpp"

template<typename T>
std::ostream& operator<<(std::ostream& out, const Fraction<T>& fraction) {
	out << fraction.numerator();
	if (fraction.denominator() != 1 && fraction.numerator() != 0) {
		out << '/' << fraction.denominator();
	}
	return out;
}

template<typename T, std::size_t Size>
std::ostream& operator<<(std::ostream& out, const Matrix<T, Size>& matrix) {
	for (std::size_t i{ 1 }; const T val : matrix) {
		out << val << ' ';
		if (i++ == Size) {
			i = 1;
			out << std::endl;
		}
		else {
			out << ' ';
		}
	}
	return out;
}

consteval Matrix<double, 5> matrix() {
	return Matrix<double, 5>({
		0, 0, 1, 1, 1,
		1, 0, 0, 1, 1,
		0, 1, 1, 0, 0,
		0, 0, 0, 1, 1,
		1, 1, 0, 1, 0
	});
}

int main() {
	std::cout << Fraction<Fraction<int>>(Fraction<int>(2, 3), Fraction<int>(2, 3));
}