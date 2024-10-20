#pragma once
#include <array>
#include <iosfwd>
#include <type_traits>
#include "Math.hpp"
#include <iostream>

template<typename T, std::size_t Size>
requires (std::is_arithmetic_v<T> || std::is_base_of_v<MathType, T>)
class Matrix : public MathType {
	static_assert(Size > 0, "Size must be at least 1*1 !");

	static constexpr std::size_t nCoefs{ Size * Size };
	using vals_t = std::array<T, nCoefs>;
	vals_t coefs_{};

	constexpr Matrix<T, Size - 1> subMatrix(std::size_t lineToErase, std::size_t columnToErase) const noexcept {
		std::array<T, (Size - 1)* (Size - 1)> subCoefs{};
		std::size_t coefsCopied{};
		for (std::size_t line{}; line < Size; line++) {
			for (std::size_t column{}; column < Size; column++) {
				if (line != lineToErase && column != columnToErase) {
					subCoefs[coefsCopied++] = coefs_[line * Size + column];
				}
			}
		}
		return Matrix<T, Size - 1>(std::move(subCoefs));
	}

public:
	static consteval Matrix identity() noexcept {
		vals_t coefs{};
		for (std::size_t i{}; i < Size; i++) {
			coefs[i * (Size + 1)] = 1;
		}
		return Matrix(std::move(coefs));
	}

	explicit constexpr Matrix(const vals_t& coefs) noexcept : coefs_{ coefs } {}

	explicit constexpr Matrix(vals_t&& coefs) noexcept : coefs_{ coefs } {}

	constexpr Matrix(T value = 0) noexcept {
		for (auto& val : coefs_) {
			val = value;
		}
	}

	constexpr Matrix(const Matrix<T, Size>& other) noexcept : coefs_{ other.coefs_ } {}

	constexpr void fillWith(T value) noexcept {
		for (auto& val : coefs_) {
			val = value;
		}
	}

	constexpr void increaseAllCoefs(T value) noexcept(noexcept(T() += T())) {
		for (auto& val : coefs_) {
			val += value;
		}

	}

	constexpr void decreaseAllCoefs(T value) noexcept(noexcept(T() += T())) {
		for (auto& val : coefs_) {
			val -= value;
		}
	}

	// if NewSize is bigger than Size, extra coefs will be zero-initialized
	template<std::size_t NewSize>
	constexpr Matrix<T, NewSize> resized() const noexcept {
		if constexpr (Size == NewSize) {
			return *this;
		}
		else {
			Matrix<T, NewSize> result{ 0 };
			const auto smallestSize{ std::min(Size, NewSize) };
			for (std::size_t line{}; line < smallestSize; line++) {
				for (std::size_t column{}; column < smallestSize; column++) {
					result.at(line, column) = at(line, column);
				}
			}
			return result;
		}
	}

	constexpr auto begin() const noexcept {
		return coefs_.begin();
	}

	constexpr auto end() const noexcept {
		return coefs_.end();
	}

	template<std::size_t line, std::size_t column>
	requires (line < Size&& column < Size)
	consteval T at() const noexcept {
		return coefs_[line * Size + column];
	}

	constexpr T at(std::size_t line, std::size_t column) const {
		return coefs_.at(line * Size + column);
	}

	constexpr T at(std::size_t index) const {
		return coefs_.at(index);
	}

	constexpr T& at(std::size_t line, std::size_t column) {
		return coefs_.at(line * Size + column);
	}

	constexpr T& at(std::size_t index) {
		return coefs_.at(index);
	}

	constexpr T det() const noexcept {
		if constexpr (Size == 1) {
			return coefs_[0];
		}
		else if constexpr (Size == 2) {					// | a  b |
			return coefs_[3] * coefs_[0] - coefs_[1] * coefs_[2];	// | c  d |  = ad - bc
		}
		else {
			T determinant{};
			for (std::size_t i{}; i < Size; i++) {
				const auto subMatrixDet{ subMatrix(0, i).det() };
				if (i % 2 == 0) {
					determinant += coefs_[i] * subMatrixDet;
				}
				else {
					determinant -= coefs_[i] * subMatrixDet;
				}
			}
			return determinant;
		}

		// How to compute determinant of a N*N matrix :
		// https://www.dcode.fr/determinant-matrice
		// https://www.youtube.com/watch?v=rCHvmB-Oei4&ab_channel=Math%C3%A9ma-TIC
	}

	template<std::size_t line>
	requires(line < Size)
		consteval T lineSum() const noexcept {
		T sum{};
		for (std::size_t column{}; column < Size; column++) {
			sum += at<line, column>();
		}
		return sum;
	}

	constexpr T lineSum(std::size_t line) const {
		T sum{};
		for (std::size_t column{}; column < Size; column++) {
			sum += at(line, column);
		}
		return sum;
	}

	template<std::size_t column>
	requires(column < Size)
		consteval T columnSum() const noexcept {
		T sum{};
		for (std::size_t line{}; line < Size; line++) {
			sum += at<line, column>();
		}
		return sum;
	}

	constexpr T columnSum(std::size_t column) const {
		T sum{};
		for (std::size_t line{}; line < Size; line++) {
			sum += at(line, column);
		}
		return sum;
	}

	constexpr T sum() const noexcept {
		T sum{};
		for (const auto val : coefs_) {
			sum += val;
		}
		return sum;
	}

	constexpr auto transpose() const noexcept {
		vals_t newCoefs{};
		for (std::size_t line{}; line < Size; line++) {
			for (std::size_t column{}; column < Size; column++) {
				const auto coef{ coefs_[line * Size + column] };
				newCoefs[column * Size + line] = coef;
			}
		}
		return Matrix(newCoefs);
	}

	constexpr bool isInvertible() const noexcept {
		return det() != 0;
	}

	// may be quite long at runtime, according to the size
	constexpr bool isNilpotent(std::size_t depth = Size) const noexcept {
		if (depth == 0) {
			return coefs_ = vals_t{};
		}
		return (*this * (*this)).isNilpotent(--depth);
	}

	template<typename U>
	requires ((std::is_arithmetic_v<U> && std::is_convertible_v<T, U>) || std::is_base_of_v<MathType, U>)
	constexpr auto convertTo() const noexcept {
		std::array<U, Size * Size> newCoefs{};
		for (std::size_t i{}; const T val : coefs_) {
			newCoefs[i++] = U(val);
		}
		return Matrix<U, Size>(std::move(newCoefs));
	}

	constexpr Matrix<long double, Size> inverse() const noexcept {
		if constexpr (Size == 1) {
			return Matrix<T, Size>({ static_cast<T>(1.L / coefs_[0]) });
		}

		const long double factor{ 1.L / det() };
		std::array<long double, nCoefs> comatrix{};
		for (std::size_t line{}; line < Size; line++) {
			for (std::size_t column{}; column < Size; column++) {
				const auto comatrixCoef{ subMatrix(line, column).det() };
				const std::size_t index{ line * Size + column };
				if (index % 2 == 0) {
					comatrix[index] = comatrixCoef;
				}
				else {
					comatrix[index] = -comatrixCoef;
				}
			}
		}
		return (factor * Matrix<long double, Size>(std::move(comatrix)).transpose());
	}

	constexpr auto operator-() const noexcept {
		auto newCoefs{ coefs_ };
		for (auto& val : newCoefs) {
			val = -val;
		}
		return Matrix<T, Size>(std::move(newCoefs));
	}

	constexpr auto operator^(std::size_t pow) {
		return power(*this, pow);
	}

	template<typename T_, std::size_t Size_>
	constexpr friend auto operator+(const Matrix<T_, Size_>& first, const Matrix<T_, Size_>& second) noexcept;

	template<typename T_, std::size_t Size_>
	constexpr friend auto operator-(const Matrix<T_, Size_>& first, const Matrix<T_, Size_>& second) noexcept;

	template<typename T_, std::size_t Size_>
	constexpr friend auto operator*(const Matrix<T_, Size_>& matrix, T_ factor) noexcept;

	template<typename T_, std::size_t Size_>
	constexpr friend auto operator*(T_ factor, const Matrix<T_, Size_>& matrix) noexcept;

	template<typename T_, std::size_t Size_>
	constexpr friend auto operator*(const Matrix<T_, Size_>& first, const Matrix<T_, Size_>& second) noexcept;
};

template<typename T_, std::size_t Size_>
constexpr auto operator+(const Matrix<T_, Size_>& first, const Matrix<T_, Size_>& second) noexcept {
	auto sum{ first.coefs_ };
	for (std::size_t i{}; auto & val : sum) {
		val += second.coefs_[i++];
	}
	return Matrix<T_, Size_>(std::move(sum));
}

template<typename T_, std::size_t Size_>
constexpr auto operator-(const Matrix<T_, Size_>& first, const Matrix<T_, Size_>& second) noexcept {
	return first + -second;
}

template<typename T_, std::size_t Size_>
constexpr auto operator*(const Matrix<T_, Size_>& matrix, T_ factor) noexcept {
	auto productCoefs{ matrix.coefs_ };
	for (auto& val : productCoefs) {
		val *= factor;
	}
	return Matrix<T_, Size_>(std::move(productCoefs));
}

template<typename T_, std::size_t Size_>
constexpr auto operator*(T_ factor, const Matrix<T_, Size_>& matrix) noexcept {
	return matrix * factor;
}

template<typename T_, std::size_t Size_>
constexpr auto operator*(const Matrix<T_, Size_>& first, const Matrix<T_, Size_>& second) noexcept {
	std::array<T_, Size_ * Size_> productCoefs{};
	for (std::size_t line{}, column{}; auto & val : productCoefs) {
		for (std::size_t i{}; i < Size_; i++) {
			val += first.coefs_[line * Size_ + i] * second.coefs_[i * Size_ + column]; // 1st Matrix's line multiplied by 2nd Matrix's column
		}

		if (++column == Size_) {
			column = 0;
			line++;
		}
	}
	return Matrix<T_, Size_>(productCoefs);
}

template<typename T, std::size_t Size>
constexpr Matrix I{ Matrix<T, Size>::identity() };
