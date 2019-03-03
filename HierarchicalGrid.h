#pragma once

#include <iostream>
#include "Settings.h"
#include "Array.h"
#include "Pool.h"
#include "AABB.h"
#include "BroadPhase.h"

// represents one collider in simulation
struct HierarchicalGridNode{
	AABB aabb; // this node's AABB
	ushort indeces[4]; // indeces of nodeInCells
	int data; // user data that uniquely identifies collider (used to return collisions back to the user)
	int level; // index of the level this node is in
	ushort stamp; // time stamp to avoid duplicate collisions
	inline HierarchicalGridNode();
};
inline HierarchicalGridNode::HierarchicalGridNode(){
	for(int i = 0; i < 4; ++i){
		indeces[i] = (ushort)-1;
	}
}

// one for every cell each node is in
struct HierarchicalGridNodeInCell{
	ushort previous; // "pointer" to previous NodeInCell in the bucket of the hash table
	ushort next; // "pointer" to next NodeInCell in the bucket of the hash table
	ushort node; // index of corresponding node in the list of nodes
	ushort cell; // stored hash of the cell
	inline HierarchicalGridNodeInCell();
};
HierarchicalGridNodeInCell::HierarchicalGridNodeInCell(){
	previous = (ushort)-1;
	next = (ushort)-1;
	node = (ushort)-1;
	cell = (ushort)-1;
}

// some kind of simple hash table where each bucket points to the linked list of NodeInCells
struct HierarchicalGridLevel{
	ushort* cells; // array of buckets (note that more than one cell can map to the same bucket, in that case all their nodeInCells are added to the same bucket)
	int capacity; // number of buckets
	int count; // number of nodeInCells in that level

	inline HierarchicalGridLevel(int initialCapacity);
	inline ~HierarchicalGridLevel();
	inline ushort get(int cell); // returns the first nodeInCell in the bucket with given hash
	inline void set(int cell, ushort nodeInCell); // sets the first nodeInCell in the bucket with given hash
	inline float density(); // calculates density of that level (count/capacity)
};
HierarchicalGridLevel::HierarchicalGridLevel(int initialCapacity){
	count = 0;
	capacity = initialCapacity;
	cells = new ushort[capacity];
	for(int i = 0; i < capacity; ++i){
		cells[i] = (ushort)-1;
	}
}
HierarchicalGridLevel::~HierarchicalGridLevel(){
	delete[] cells;
}
ushort HierarchicalGridLevel::get(int cell){
	return cells[cell % capacity];
}
void HierarchicalGridLevel::set(int cell, ushort nodeInCell){
	cells[cell % capacity] = nodeInCell;
}
float HierarchicalGridLevel::density(){
	return (float)count / (float)capacity;
}

class HierarchicalGrid: public BroadPhase{
private:
	static constexpr ushort null = (ushort)-1;
	Array<HierarchicalGridNode> nodes; // array of nodes. If element is removed from this array, the last element is moved to its spot.
	Stack<HierarchicalGridLevel*> levels; // array of levels
	Pool<HierarchicalGridNodeInCell> nodesInCells; // array of NodeInCells

	const int initialLevelCapacity = 256; // initial number of buckets of each level
	const float levelMaxDensity = 0.6f; // max density of hash table before it is resized
	const float nodeToCell = 1.1f; // ratio between level's cell size and AABB size
	const float levelToLevel = 4.0f; // ratio between cell sizes of two adjacent levels
	const float firstLevelCellSize; // cell size of the first level (the one with the smallest cells)

	inline int cellId(int x, int y); // computes hash from cell's coordinates
	void rehash(int level); // NodeInCells that belong to given level are inserted into it. Called after level has changed capacity
public:
	inline HierarchicalGrid(const float initialCellSize, const float levelToLevel);
	inline ~HierarchicalGrid();

	// creates new node and inserts it into the grid. Returns id by which the node can be identified.
	int add(AABB& aabb, int data);

	// Removes given node. It is possible that onother node changes id.
	// In that case the node (data) and its new id (newId) are communicated back.
	// Returns true if a node changes id, false otherwise.
	bool remove(int id, int& data, int& newId);

	// moves nodes AABB for the given translation and corrects the grid
	void update(int id, Vec& translation);

	// removes all collisions and adds current ones to the list
	void getCollisions();

	bool check();
	bool check1();
	bool check2();
	void levelCounts();

	// prints data to get an insight on what is going on
	void print();
};

inline int HierarchicalGrid::cellId(int x, int y){
	return ((179424691*x)^(15485863*y)) % 1000000007 + 1000000007;
}

HierarchicalGrid::HierarchicalGrid(const float initialCellSize, const float levelToLevel)
	: nodes(128), levels(4), nodesInCells(128), firstLevelCellSize(initialCellSize), levelToLevel(levelToLevel){
	levels.push(new HierarchicalGridLevel(initialLevelCapacity));
}
HierarchicalGrid::~HierarchicalGrid(){
	for(int i = 0; i < levels.count; ++i){
		delete levels[i];
	}
}
