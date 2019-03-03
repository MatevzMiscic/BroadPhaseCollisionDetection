#pragma once

#include <string>
#include <math.h>
#include "Math.h"

struct Vec {
	float x;
	float y;

	inline Vec() = default;
	inline Vec(float X, float Y);

	inline Vec(const Vec& vec) = default;
	inline Vec(Vec&& vec) = default;

	inline Vec& operator= (const Vec& vec) = default;
	inline Vec& operator= (Vec&& vec) = default;

	// set vector with given coordinates
	inline void set(float X, float Y);

	// adds given vector
	inline void operator+= (const Vec& vec);

	// subtracts given vector
	inline void operator-= (const Vec& vec);

	// multiplies vector with given scalar
	inline void operator*= (float scalar);

	// divides vector with given scalar
	inline void operator/= (float scalar);

	// makes unit vector from this vector
	inline void normalize();

	// returns lenght of the vector
	inline float length() const;

	// returns lenght squared of the vector
	inline float length2() const;

	// returns unit vector of this vector
	inline Vec unit() const;

	// returns vector that is a 90 degree rotation of this vector
	inline Vec left() const;

	// returns vector that is a 270 degree rotation of this vector
	inline Vec right() const;

	// returns vector that is a rotation of this vector given the sine and cosine of the angle
	inline Vec rotate(float sin, float cos) const;

	// returns string representation of this vector
	inline std::string toString() const;
};

// returns the sum of given vectors
inline Vec operator+ (const Vec& a, const Vec& b);

// returns the difference of given vectors
inline Vec operator- (const Vec& a, const Vec& b);

// returns multiplication of given vector and a scalar
inline Vec operator* (const Vec& vec, float scalar);

// returns division of given vector and a scalar
inline Vec operator/ (const Vec& vec, float scalar);

// returns vector with min coordinates of given vectors
inline Vec Min(const Vec& a, const Vec& b);

// returns vector with max coordinates of given vectors
inline Vec Max(const Vec& a, const Vec& b);

// returns vector closest to vec with x and y coordinates between min and max x and y coordinates
// x and y min coordinates should be less or equal to x and y max coordinates respectively
inline Vec Cut(const Vec& min, const Vec& vec, const Vec& max);

// returns negation of given vector
inline Vec operator- (const Vec& vec);

// returns dot product between given vectors
inline float Dot(const Vec& a, const Vec& b);

// returns cross product between given vectors
// scalar represents vector that is perpendicular to the plane
inline float Cross(const Vec& a, const Vec& b);
inline Vec Cross(const Vec& vec, float scalar);
inline Vec Cross(float scalar, const Vec& vec);


Vec::Vec(float X, float Y){
	x = X;
	y = Y;
}
inline void Vec::set(float X, float Y){
	x = X;
	y = Y;
}

inline void Vec::operator+= (const Vec& vec){
	x += vec.x;
	y += vec.y;
}
inline void Vec::operator-= (const Vec& vec){
	x -= vec.x;
	y -= vec.y;
}
inline void Vec::operator*= (float scalar){
	x *= scalar;
	y *= scalar;
}
inline void Vec::operator/= (float scalar){
	float invScalar = 1 / scalar;
	x *= invScalar;
	y *= invScalar;
}

inline void Vec::normalize(){
	float invLen = 1 / sqrt(x*x + y*y);
	x *= invLen;
	y *= invLen;
}
inline float Vec::length() const{
	return sqrt(x*x + y * y);
}
inline float Vec::length2() const{
	return x * x + y * y;
}

inline Vec Vec::unit() const{
	float invLen = 1 / sqrt(x*x + y * y);
	return Vec(x*invLen, y*invLen);
}
inline Vec Vec::left() const{
	return Vec(-y, x);
}
inline Vec Vec::right() const{
	return Vec(y, -x);
}
inline Vec Vec::rotate(float sin, float cos) const{
	Vec(x*cos - y * sin, x*sin + y * cos);
}

inline std::string Vec::toString() const{
	return std::to_string(x) + ", " + std::to_string(y);
}

// non-member functions
inline Vec operator+ (const Vec& a, const Vec& b){
	return Vec(a.x + b.x, a.y + b.y);
}
inline Vec operator- (const Vec& a, const Vec& b){
	return Vec(a.x - b.x, a.y - b.y);
}
inline Vec operator* (const Vec& vec, float scalar){
	return Vec(vec.x*scalar, vec.y*scalar);
}
inline Vec operator/ (const Vec& vec, float scalar){
	float invScalar = 1 / scalar;
	return Vec(vec.x*invScalar, vec.y*invScalar);
}

inline Vec Min(const Vec& a, const Vec& b){
	return Vec(Math::min(a.x, b.x), Math::min(a.y, b.y));
}
inline Vec Max(const Vec& a, const Vec& b){
	return Vec(Math::max(a.x, b.x), Math::max(a.y, b.y));
}
inline Vec Cut(const Vec& min, const Vec& vec, const Vec& max){
	return Vec(Math::cut(min.x, vec.x, max.x), Math::cut(min.y, vec.y, max.y));
}

inline Vec operator- (const Vec& vec){
	return Vec(-vec.x, -vec.y);
}

inline float Dot(const Vec& a, const Vec& b){
	return a.x*b.x + a.y*b.y;
}
inline float Cross(const Vec& a, const Vec& b){
	return a.x*b.y - a.y*b.x;
}
inline Vec Cross(const Vec& vec, float scalar){
	return Vec(vec.y*scalar, -vec.x*scalar);
}
inline Vec Cross(float scalar, const Vec& vec){
	return Vec(-vec.y*scalar, vec.x*scalar);
}