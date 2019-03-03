#pragma once

#include <iostream>
#include "Settings.h"
#include "Math.h"
#include "Pool.h"
#include "Random.h"
#include "Vector.h"
#include "AABB.h"
#include "BroadPhase.h"
#include "BruteForce.h"
#include "Tree.h"
#include "UniformGrid.h"
#include "HierarchicalGrid.h"
#include "SweepAndPrune.h"

struct Collider{
	AABB aabb; // collider's bounding box
	Vec velocity; // collider's velocity
	int id[6]; // ids are used to tell broadphase which collider is to be modified
};


// simulates environment with given parameters
class World{
private:
	int points = 2; // number of higher density points (points around which more colliders are located)
	float clusterX[2]; // x coordinates of those points
	float clusterY[2]; // y coordinates of those points
	
	int colliders = 1000; // number of colliders
	float surface = 10000.0f; // world's surface area
	float worldRatio = 1.0f; // ratio between world's sizes in x and y direction
	float density = 0.5f; // ratio between combined area of all colliders and world area
	float diversity = 1.0f; // ratio between colliderMaxSize and colliderMinSize
	float destroyRate = 0.4f; // percentage of destroyed and rebuilt colliders each frame
	float uniformity = 1.0f; // measure of how uniformly distributed are objects (value of 1.0 means object are distributed evenly)
	float colliderRatio = 10.0; // max ratio between longer and shorter side of collider

	float sizeX; // world size in x direction
	float sizeY; // world size in y direction
	float colliderMinSize; // min length of AABB's shorter side
	float colliderMaxSize; // max length of AABB's shorter side

	float velocity = 0.1f; // max collider's velocity in terms of its size
	float damping = 0.001f; // how much velocity is damped
	float acceleration = 0.1f; // max collider's accceleration in terms of its size

	Pool<Collider> pool; // list of colliders

	Random gen; // random number generator
	AABB aabb(); // creates and returns random AABB according to world's parameters
	Vec vector(float length); // returns vector with given length pointing in random direction

	int methodCount = 5; // number of broad phase methods
	BroadPhase* method[6]; // array of broad phase methods
	
	float distribution(float x, float y); // returns probability that a collider is spawned at given position

	void setUniformGrid(int accuracy, int reliability); // finds best cell size for uniform grid
	void setHierarchicalGrid(int reliability); // finds the best cell ratio between adjacent levels
	void setSweepAndPrune(int accuracy, int reliability); // finds when quicksort should fall top insertion sort for best performance

	static bool compare(const Pair<int>& a, const Pair<int>& b); // compares two pairs
	static void order(Stack<Pair<int>>& stack); // Sorts stack. It is used to sort collision lists so that they can be compared
	bool validate(Stack<Pair<int>>& stack, Stack<Pair<int>>& stack1); // checks whether two sorted stacks are the same
	void print();
public:
	// sap: true if results for insertion sort sweep and prune should be shown, false otherwise
	// n: number of colliders simulated
	// ratio: ratio between world's sizes in x and y direction
	// density: ratio between combined area of all colliders and world area
	// diversity: ratio between colliderMaxSize and colliderMinSize
	// destroyRate: percentage of destroyed and rebuilt colliders each frame
	// uniformity: measure of how uniformly distributed are objects
	// colliderRatio: max ratio between longer and shorter side of collider
	// velocity: max collider's velocity in terms of its size
	World(bool sap, int n, float ratio, float density, float diversity, float destroyRate, float uniformity, float colliderRatio, float velocity);
	~World();

	void test(); // tests different collision detection algorithms

	void run(int iter); // runs the simulation for given number of iterations
	bool time(int iter); // times different methods over given number of iterations and stores their times
	long long times[6]; // stored times of different algorithms
};
