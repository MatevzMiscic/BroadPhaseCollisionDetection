#pragma once

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include"Settings.h"
#include "Pair.h"
#include "Stack.h"
#include "Array.h"
#include "Pool.h"
#include "AABB.h"
#include "BroadPhase.h"

// represents one collider in simulation
struct SweepAndPruneNode{
	AABB aabb; // this node's AABB
	int data; // user data that uniquely identifies collider (used to return collisions back to the user)
	ushort pointer; // index of the pointer back to this node in the array of pointers
	inline SweepAndPruneNode();
};
SweepAndPruneNode::SweepAndPruneNode(){

}

class SweepAndPrune: public BroadPhase{
private:
	Pool<SweepAndPruneNode> nodes; // array of nodes
	Array<ushort> pointers; // array of pointers
	int useInsertionSort; // max number of elements to use insertion sort

	// returns true if the node pointed to by a is to the left of the node pointed to by b
	// nodes are compared by x coordinate of min corner of their AABB
	inline bool compare(const ushort a, const ushort b);

	// sorts pointers in the given range by insertion sort
	void insertionsort(const int lo, const int hi);

	// partiotions pointers in the given range
	int partition(const int lo, const int hi);

	// sorts pointers in the given range by quicksort
	void quicksort(const int lo, const int hi);

	// sorts pointers and updates pointer indeces in nodes
	inline void sort();

	void check();
public:
	inline SweepAndPrune(int useInsertionSort);
	inline ~SweepAndPrune();

	// creates new node. Returns id by which the node can be identified.
	inline int add(AABB& aabb, int data);

	// removes given node
	// parameters data and newId are not used
	// returns false
	inline bool remove(int id, int& data, int& newId);

	// moves nodes AABB for given amount
	inline void update(int id, Vec& translation);

	// removes all collisions and adds current ones to the list
	void getCollisions();
};

SweepAndPrune::SweepAndPrune(int useInsertionSort): nodes(64), pointers(64){
	this->useInsertionSort = useInsertionSort;
}

SweepAndPrune::~SweepAndPrune(){

}

bool SweepAndPrune::compare(const ushort a, const ushort b){
	return nodes[a].aabb.min.x < nodes[b].aabb.min.x;
}

void SweepAndPrune::sort(){
	quicksort(0, pointers.count - 1);
	for(int i = 0; i < pointers.count; ++i){ // for every pointer
		nodes[pointers[i]].pointer = i; // update back pointer in the node to point to the right position
	}
}

int SweepAndPrune::add(AABB& aabb, int data){
	int nodeId = nodes.allocate(); // add a new node to nodes
	SweepAndPruneNode& node = nodes[nodeId];
	node.aabb = aabb;
	node.data = data;
	node.pointer = (ushort)pointers.allocate(); // add a new pointer to pointers
	pointers[node.pointer] = nodeId;
	return nodeId;
}
bool SweepAndPrune::remove(int id, int& data, int& newId){
	int pointerIndex = nodes[id].pointer;
	if(pointers.deallocate(pointerIndex)){ // if last pointer has changed its position
		nodes[pointers[pointerIndex]].pointer = pointerIndex; // update back pointer in the node to point to the right position
	}
	nodes.deallocate(id); // deallocate node
	return false;
}
void SweepAndPrune::update(int id, Vec& translation){
	nodes[id].aabb.translate(translation); // translate aabb
}
