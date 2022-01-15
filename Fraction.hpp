#pragma once
#include <type_traits>
#include "Math.hpp"

template<typename T>
concept isFraction = requires (T fraction) {
	fraction.numerator();
	fraction.denominator();
};

template<typename T>
requires(std::is_integral_v<T> || std::is_base_of_v<MathType, T>)
class Fraction : public MathType {
	T numerator_{};
	T denominator_{ 1 };

	constexpr void reduce() {
		if constexpr (!isFraction<T>) {
			while (true) {
				const std::optional<T> divisor{ commonDivisor<T>(numerator_, denominator_) };
				if (!divisor.has_value()) {
					break;
				}
				numerator_ /= divisor.value();
				denominator_ /= divisor.value();
			}
		}
		else {
			const auto division{ numerator_ / denominator_ };
			numerator_ = division.numerator();
			denominator_ = division.denominator();
		}
	}

public:
	/*template<typename U>
	requires(std::is_integral_v<U>)
	constexpr static auto convertToFractionOfIntegers() noexcept {
		
	}*/

	constexpr explicit Fraction(T numerator, T denominator) noexcept {
		if constexpr (isFraction<T>) {
			*this = numerator / denominator;
		}
		else {
			numerator_ = numerator;
			denominator_ = denominator;
			reduce();
		}
	}

	constexpr Fraction(T result = 0) noexcept {
		numerator_ = result;
	}

	constexpr Fraction(const Fraction<T>& other) noexcept {
		numerator_ = other.numerator_;
		denominator_ = other.denominator_;
	}

	constexpr T result() const noexcept(noexcept(T() / T())) {
		return numerator_ / denominator_;
	}

	constexpr T numerator() const noexcept {
		return numerator_;
	}

	constexpr T denominator() const noexcept {
		return denominator_;
	}

	constexpr auto inverse() const noexcept {
		return Fraction<T>(denominator_, numerator_);
	}

	constexpr auto operator+(const Fraction<T>& other) const noexcept {
		if (denominator_ == other.denominator_) {
			return Fraction<T>(numerator_ + other.numerator_, denominator_);
		}
		const T commonDenominator{ denominator_ * other.denominator_ };
		const T correspondingNumerator{ numerator_ * other.denominator_ + other.numerator_ * denominator_ };
		return Fraction<T>(correspondingNumerator, commonDenominator);
	}

	constexpr auto operator+(T other) const noexcept {
		return *this + Fraction<T>(other);
	}

	constexpr auto operator-() const noexcept {
		return Fraction<T>(-numerator_, denominator_);
	}

	constexpr auto operator-(const Fraction<T>& other) const noexcept {
		return *this + -other;
	}

	constexpr auto operator-(T other) const noexcept {
		return *this - Fraction<T>(other);
	}

	constexpr auto operator*(const Fraction<T>& other) const noexcept {
		return Fraction<T>(numerator_ * other.numerator_, denominator_ * other.denominator_);
	}

	constexpr auto operator*(T other) const noexcept {
		return Fraction<T>(numerator_ * other, denominator_);
	}

	constexpr auto operator/(const Fraction<T>& other) const noexcept {
		return *this * other.inverse();
	}

	constexpr auto operator/(T other) const noexcept {
		return Fraction<T>(numerator_, denominator_ * other);
	}

	constexpr auto operator^(unsigned long long pow) const noexcept {
		return Fraction<T>(power(numerator_, pow), power(denominator_, pow));
	}

	constexpr auto& operator+=(const Fraction<T>& other) noexcept {
		return (*this = *this + other);
	}

	constexpr auto& operator+=(T other) noexcept {
		return (*this = *this + other);
	}

	constexpr auto& operator++() noexcept {
		return (*this += 1);
	}

	constexpr auto& operator++(int) noexcept {
		auto& old{ *this };
		++(*this);
		return *this;
	}

	constexpr auto& operator-=(const Fraction<T>& other) noexcept {
		return (*this = *this - other);
	}

	constexpr auto& operator-=(T other) noexcept {
		return (*this = *this - other);
	}

	constexpr auto& operator--() noexcept {
		return (*this -= 1);
	}

	constexpr auto& operator--(int) noexcept {
		auto& old{ *this };
		--(*this);
		return *this;
	}

	constexpr auto& operator*=(const Fraction<T>& other) noexcept {
		return (*this = *this * other);
	}

	constexpr auto& operator*=(T other) noexcept {
		return (*this = *this * other);
	}

	constexpr auto& operator/=(const Fraction<T>& other) noexcept {
		return (*this = *this / other);
	}

	constexpr auto& operator/=(T other) noexcept {
		return (*this = *this / other);
	}

	constexpr auto& operator^=(unsigned long long pow) noexcept {
		return (*this = *this ^ pow);
	}

	template<typename U>
	requires (std::is_arithmetic_v<U> && !std::is_same_v<T, U> && std::is_convertible_v<T, U>)
	constexpr operator U() const noexcept {
		return Fraction<U>(static_cast<U>(numerator_), static_cast<U>(denominator_)).result();
	}

	constexpr bool operator==(const Fraction<T>& other) const noexcept {
		return numerator_ == other.numerator() && denominator_ == other.denominator();
	}

	constexpr bool operator==(T result) const noexcept {
		return this->result() == result;
	}

	constexpr bool operator!=(const Fraction<T>& other) const noexcept {
		return !(*this == other);
	}

	constexpr bool operator!=(T result) const noexcept {
		return !(*this == result);
	}
};

template<typename T>
constexpr auto operator+(T first, const Fraction<T>& other) noexcept {
	return other + first;
}

template<typename T>
constexpr auto operator-(T first, const Fraction<T>& other) noexcept {
	return Fraction<T>(first) - other;
}

template<typename T>
constexpr auto operator*(T first, const Fraction<T>& other) noexcept {
	return other * first;
}

template<typename T>
constexpr auto operator/(T first, const Fraction<T>& other) noexcept {
	return Fraction<T>(first) / other;
}

template<typename T>
constexpr auto operator/(const Fraction<T>& divided, T divisor) noexcept {
	return Fraction<T>(divided.numerator(), divided.denominator() * divisor);
}

template<typename T>
constexpr bool operator==(T result, const Fraction<T>& other) noexcept {
	return result == other.result();
}

template<typename T>
constexpr bool operator!=(T result, const Fraction<T>& other) noexcept {
	return !(result == other);
}