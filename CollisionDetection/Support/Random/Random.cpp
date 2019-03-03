#include <iostream>
#include "Random.h"
#include "Math.h"

Random::Random(){
	uint seed = (uint)time(0);
	srand(seed);
}
Random::Random(uint seed){
	srand(seed);
}
float Random::random(){
	return (float)rand()/(float)(RAND_MAX + 1);
}
float Random::random(float min, float max){
	return min + (max - min)*random();
}
float Random::randomExp(float min, float max){
	return min * Math::pow(max/min, random());
}
