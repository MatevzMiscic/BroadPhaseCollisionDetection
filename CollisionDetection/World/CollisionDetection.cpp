#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstdlib>
#include <ctime>
#include "Vector.h"
#include "AABB.h"
#include "BruteForce.h"
#include "World.h"

using namespace std;

bool sap = false;
int colliders = 1000; // number of colliders
float worldRatio = 1.0f; // ratio between world's sizes in x and y direction
float density = 0.8f; // ratio between combined area of all colliders and world area (from 0 to infinity)
float diversity = 1.0f; // ratio between colliderMaxSize and colliderMinSize (from 1 to infinity)
float destroyRate = 0.4f; // aproximate percentage of destroyed and recreated colliders each frame (from 0 to 1)
float uniformity = 1.0f; // measure of how uniformly distributed are objects (from 0 to 1)
float colliderRatio = 1.0; // max ratio between longer and shorter side of collider (from 1 to infinity)
float velocity = 0.1;

int run = 10; // number of iterations run before timing so that environment stabilizes
int iterations = 40; // number of timed iterations

// linearly interpolates given variable between given values and prints time taken by each method for every value
void linear(float* var, float min, float max, float step){
	float copy = *var;
	for(float value = min; value <= max; value += step){
		*var = value;
		World world(sap, colliders, worldRatio, density, diversity, destroyRate, uniformity, colliderRatio, velocity);
		world.run(run);
		world.time(iterations);
		std::cout << value << "\t";
		for(int i = 0; i < 5; ++i){
			std::cout << std::setw(10) << world.times[i] << "\t";
		}
		if(sap){
			std::cout << world.times[5] << "\t";
		}
		std::cout << "\n";
	}
	*var = copy;
}
void exponential(float* var, float min, float max, float step, float minStep){
	float copy = *var;
	for(float value = min; value <= max; value = Math::max(value*step, minStep)){
		*var = value;
		World world(sap, colliders, worldRatio, density, diversity, destroyRate, uniformity, colliderRatio, velocity);
		world.run(run);
		world.time(iterations);
		std::cout << value << "\t";
		for(int i = 0; i < 5; ++i){
			std::cout << std::setw(10) << world.times[i] << "\t";
		}
		if(sap){
			std::cout << world.times[5] << "\t";
		}
		std::cout << "\n";
	}
	*var = copy;
}

// linearly interpolates number of colliders between given values and prints time taken by each method for every value
void graphColliderCount(int min, int max, int step){
	for(int n = min; n <= max; n += step){
		World world(sap, n, worldRatio, density, diversity, destroyRate, uniformity, colliderRatio, velocity);
		world.run(run);
		world.time(iterations);
		std::cout << n << "\t";
		for(int i = 0; i < 5; ++i){
			std::cout << std::setw(10) << world.times[i] << "\t";
		}
		if(sap){
			std::cout << world.times[5] << "\t";
		}
		std::cout << "\n";
	}
}

int main(){

	// time (colliders)
	std::cout << "\nNumber of colliders:\n";
	graphColliderCount(100, 900, 100);
	graphColliderCount(1000, 10000, 1000);

	// time (diversity)
	linear(&diversity, 1.0f, 10001.0f, 1000.0f);

	// time (insertions and delitions)
	std::cout << "\nNumber of delitions and reinsertions:\n";
	sap = true;
	linear(&destroyRate, 0.0f, 0.51f, 0.05f);
	sap = false;

	// time (colliderRatio)
	std::cout << "\nRatio between longer and shorter collider side:\n";
	linear(&colliderRatio, 1.0f, 141.0f, 20.0f);

	// time (velocity)
	std::cout << "\nVelocity:\n";
	destroyRate = 0.0f;
	sap = true;
	linear(&velocity, 0.0f, 10.1f, 1.0f);
	sap = false;

	cin.get();
}