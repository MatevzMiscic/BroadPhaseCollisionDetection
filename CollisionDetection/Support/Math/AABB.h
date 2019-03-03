#pragma once

#include <string>
#include "Vector.h"

struct AABB{
	Vec min; // bottom left corner coordinates
	Vec max; // top right corner coordinates

	AABB();
	AABB(const Vec& min, const Vec& max);

	// sets AABB to equal given AABB
	void operator= (AABB aabb);

	// expend AABB to also include given AABB
	void eat(const AABB& aabb);

	// returns true if given AABB is completly enclosed by this one
	bool isIn(const AABB& aabb);

	// moves AABB for given translation
	void translate(const Vec& translation);

	// makes AABB bigger in all four directions for goven amount
	void fatten(float tolerance);

	// returns AABB's area
	float area();

	// returns string that represents this AABB
	inline std::string toString() const;
};

inline std::string AABB::toString() const {
	return min.toString() + ", " + max.toString();
}

// returns true if AABBs share a common point
bool areColliding(AABB& a, AABB& b);

// returns the smallest possible AABB which still fully contains both given AABBs
AABB eat(AABB& a, AABB& b);

// makes given AABB bigger in all four directions for goven amount
AABB fatten(AABB& aabb, float fatness);
