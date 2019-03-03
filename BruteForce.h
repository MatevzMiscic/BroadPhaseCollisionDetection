#pragma once

#include <iostream>
#include "Vector.h"
#include "AABB.h"
#include "Pair.h"
#include "Stack.h"
#include "Array.h"
#include "BroadPhase.h"

struct BruteForceNode{
	AABB aabb;
	int data;
};

class BruteForce: public BroadPhase{
private:
	Array<BruteForceNode> nodes;
public:
	BruteForce();
	~BruteForce();
	
	// creates new node
	// returns id by which the node can be identified.
	int add(AABB& aabb, int data);

	// removes given node from the grid. It is possible that onother node changes id.
	// in that case the node (data) and its new id (newId) are communicated back
	// returns true if a node changes id, false otherwise
	bool remove(int id, int& data, int& newId);

	// moves nodes AABB for given amount
	void update(int id, Vec& translation);

	// removes all collisions and adds current ones to the list
	void getCollisions();
	
	int countCollisions();
};
