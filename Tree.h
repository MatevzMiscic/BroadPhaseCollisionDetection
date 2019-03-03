#pragma once

#include <iostream>
#include <string.h>
#include "Settings.h"
#include "Pair.h"
#include "Stack.h"
#include "Pool.h"
#include "AABB.h"
#include "BroadPhase.h"

constexpr ushort null = -1;

constexpr float fat = 0.0f; // how much fatter we make the AABB

// node in the tree
// only leaf nodes represent colliders
struct TreeNode{
	ushort parent = null; // parent node in the tree
	ushort left = null; // left child in the tree
	ushort right = null; // right child in the tree
	ushort height = null; // height of given node

	AABB aabb; // nodes AABB contains all decendants or the collider's AABB if it is a leaf
	int data; // user data that uniquely identifies collider (used to return collisions back to the user). Only leafs use data

	inline TreeNode();
	inline bool isLeaf();
};
TreeNode::TreeNode(){
	parent = null;
	left = null;
	right = null;
	height = null;
}
bool TreeNode::isLeaf(){
	return right == null;
}

class Tree: public BroadPhase{
private:
	Pool<TreeNode> nodes;
	ushort root;

	// inserts leaf into the tree
	void addLeaf(int node);

	// removes leaf from the tree
	void removeLeaf(int node);

	// ballances tree arount given node
	int balance(int node);
public:
	inline Tree();

	// creates new node and inserts it into the tree. Returns id by which the node can be identified.
	inline int add(AABB& aabb, int data);

	// removes given node from the tree
	// parameters data and newId are not used
	// returns false
	inline bool remove(int id, int& data, int& newId);

	// removes node from the tree, moves its AABB for given amount and reinserts it back into the tree
	inline void update(int id, Vec& translation);

	// removes all collisions and adds current ones to the list
	void getCollisions();

	inline int nodeCount();
};

Tree::Tree(): nodes(32){
	root = null;
}

// do not use references because they could become invalid if pool resizes. Use indeces instead.
int Tree::add(AABB& aabb, int data){
	int nodeId = nodes.allocate();
	nodes[nodeId].aabb = aabb;
	nodes[nodeId].data = data;
	nodes[nodeId].aabb.fatten(fat);
	addLeaf(nodeId);
	nodes[nodeId].aabb.fatten(-fat);
	return nodeId;
}
bool Tree::remove(int id, int& data, int& newId){
	removeLeaf(id);
	nodes.deallocate(id);
	return false;
}
// do not use references because they could become invalid if pool resizes. Use indeces instead.
void Tree::update(int id, Vec& translation){
	nodes[id].aabb.translate(translation); // translate node
	if(nodes[id].parent != null && !nodes[nodes[id].parent].aabb.isIn(nodes[id].aabb)){ // if it is not encapsulated by its parent anymore
		removeLeaf(id); // remove it
		nodes[id].aabb.fatten(fat);
		addLeaf(id); // and reinsert it
		nodes[id].aabb.fatten(-fat);
	}
}
int Tree::nodeCount(){
	return nodes.count;
}
