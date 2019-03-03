#pragma once

#include "Settings.h"
#include "Pair.h"
#include "Stack.h"
#include "Array.h"
#include "AABB.h"
#include "BroadPhase.h"

// represents one collider in simulation
struct UniformGridNode{
	AABB aabb; // this node's AABB
	int data; // user data that uniquely identifies collider (used to return collisions back to the user)
	ushort stamp; // time stamp to avoid duplicate collisions
};

struct UniformGridCell{
	uint nodeList; // index of first element in list of nodes that are in this cell
	ushort nodeCount; // number of all objects in this cell
	ushort insertedNodes; // number of objects already inserted in this cell (reset every time getCollisions is called before object are reinserted into cells)
	inline UniformGridCell();
};
UniformGridCell::UniformGridCell(){
	nodeList = 0;
	nodeCount = 0;
	insertedNodes = 0;
}

class UniformGrid: public BroadPhase{
private:
	const float width; // cell width
	const float height; // cell height
	const ushort cellsX; // number of cells in x direction
	const ushort cellsY; // number of cells in y direction
	
	Array<UniformGridNode> nodes; // array of nodes. If element is removed from this array, the last element is moved to its spot.
	UniformGridCell* cells; // every cell has index of the first element in list of nodes that are in this cell. 
	ushort* nodesInCells; // lists of nodes for every cell are consecutively written in this array
	uint nodesInCellsCapacity; // the capacity of the previous array

	inline ushort cellId(int x, int y); // returns cell index (in the array of cells) for given coordinates
public:
	inline UniformGrid(const float width, const float height, const ubyte x, const ubyte y);
	inline ~UniformGrid();

	// Creates new node and inserts it into the grid. Returns id by which the node can be identified.
	int add(AABB& aabb, int data);

	// Removes given node from the grid. It is possible that onother node changes id.
	// in that case the node (data) and its new id (newId) are communicated back
	// returns true if a node changes id, false otherwise
	bool remove(int id, int& data, int& newId);

	// moves nodes AABB for given amount and corrects the grid
	void update(int id, Vec& translation);

	// removes all collisions and adds current ones to the list
	void getCollisions();

	// prints data to get an insight on what is going on
	void print();
};

// returns cell index for given coordinates
ushort UniformGrid::cellId(int x, int y){
	short modX = x % cellsX;
	if(modX < 0){
		modX += cellsX;
	}
	short modY = y % cellsY;
	if(modY < 0){
		modY += cellsY;
	}
	return modX + modY*cellsX;
}

UniformGrid::UniformGrid(const float width, const float height, const ubyte x, const ubyte y)
	: nodes(32), width(width), height(height), cellsX(x), cellsY(y){
	cells = new UniformGridCell[x*y];
	nodesInCellsCapacity = 256;
	nodesInCells = new ushort[nodesInCellsCapacity];
}
UniformGrid::~UniformGrid(){
	delete[] cells;
	delete[] nodesInCells;
}
