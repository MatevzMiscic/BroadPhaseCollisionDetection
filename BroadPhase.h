#pragma once

#include "Stack.h"
#include "Pair.h"
#include "Vector.h"
#include "AABB.h"

// base class for broad phase collision detection methods
class BroadPhase{
public:
	// list with collisions
	Stack<Pair<int>> collisions;

	// constructor
	BroadPhase(): collisions(128){}

	// Creates new node. Returns id by which the node can be identified.
	virtual int add(AABB& aabb, int data) = 0;

	// Removes given node. It is possible that onother node changes id.
	// In that case the node (data) and its new id (newId) are communicated back.
	// Returns true if a node changes id, false otherwise.
	virtual bool remove(int id, int& data, int& newId) = 0;

	// Moves nodes AABB for the given translation.
	virtual void update(int id, Vec& translation) = 0;

	// Clears list of collisions and adds current collisions to the list.
	virtual void getCollisions() = 0;
};