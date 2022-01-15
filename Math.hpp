#pragma once
#include <type_traits>
#include <optional>

class MathType {};

template<typename T>
concept isMatrix = requires {
	T::identity();
};

template<typename T>
requires(std::is_arithmetic_v<T> || std::is_base_of_v<MathType, T>)
constexpr T power(T val, unsigned long long pow) {
	if (pow == 0) {
		if constexpr (isMatrix<T>) {
			return T::identity();
		}
		else {
			return 1;
		}
	}
	return val * power(val, --pow);
}

template<typename T>
requires(std::is_signed_v<T>)
constexpr std::optional<T> commonDivisor(T first, T second) noexcept {
	for (T i{ 2 }; i <= std::min(first, second); i++) {
		const long double division{ static_cast<long double>(first) / i };
		if (division == (first / i)) {
			return i;
		}
	}
	return std::nullopt;
}