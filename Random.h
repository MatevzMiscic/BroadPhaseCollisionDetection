#pragma once

#include <cstdlib>
#include <ctime>
#include "Settings.h"

class Random{
private:

public:

	// seeds the object with some seed
	Random();

	// seeds the object with given seed
	Random(uint seed);

	// returns random float from 0 and 1
	float random();

	// returns random float between min and max
	float random(float min, float max);

	// Returns random float between min and max.
	// For every A and B such that min < A < B < max and min < 2*A < 2*B < max,
	// the probability that the returned value is in range [A, B] is the same as
	// the probability that the returned value is in range [2*A, 2*B].
	float randomExp(float min, float max);
};
