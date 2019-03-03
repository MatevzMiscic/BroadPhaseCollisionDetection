#pragma once

#include <cmath>

#define Pi 3.14159265359f

namespace Math {
	template<class T>
	inline T min(T a, T b) {
		if (a < b) {
			return a;
		}
		return b;
	}

	template<class T>
	inline T max(T a, T b) {
		if (a > b) {
			return a;
		}
		return b;
	}

	template<class T>
	inline T cut(T min, T value, T max) {
		if (value < min) {
			return min;
		}
		if (value > max) {
			return max;
		}
		return value;
	}

	template<class T>
	inline void swap(T& a, T& b) {
		T temp = a;
		a = b;
		b = temp;
	}

	inline float pow(float base, float exponent){
		return std::pow(base, exponent);
	}

	inline float pow(float base, int exponent){
		return std::pow(base, exponent);
	}

	inline float sqrt(float value){
		return std::sqrt(value);
	}

	inline float ln(float value){
		return std::log(value);
	}

	inline float log(float value){
		return std::log2(value);
	}

	inline float log(float base, float value){
		return std::log(value)/std::log(base);
	}
}