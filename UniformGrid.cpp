#include <iostream>
#include "UniformGrid.h"
#include "Bitset.h"

int UniformGrid::add(AABB& aabb, int data){
	int nodeId = nodes.allocate();
	UniformGridNode& node = nodes[nodeId];
	node.aabb = aabb;
	node.data = data;
	node.stamp = nodeId;
	int minX = (int)floor(aabb.min.x/width);
	int minY = (int)floor(aabb.min.y/height);
	int maxX = (int)floor(aabb.max.x/width);
	int maxY = (int)floor(aabb.max.y/height);
	// increase nodeCount in cells this node overlaps
	for(int x = minX; x <= maxX; ++x){
		for(int y = minY; y <= maxY; ++y){
			UniformGridCell& cell = cells[cellId(x, y)];
			++cell.nodeCount;
		}
	}
	return nodeId;
}
bool UniformGrid::remove(int id, int& data, int& newId){
	UniformGridNode& node = nodes[id];
	int minX = (int)floor(node.aabb.min.x/width);
	int minY = (int)floor(node.aabb.min.y/height);
	int maxX = (int)floor(node.aabb.max.x/width);
	int maxY = (int)floor(node.aabb.max.y/height);
	// decrease nodeCount in cells this node overlaped
	for(int x = minX; x <= maxX; ++x){
		for(int y = minY; y <= maxY; ++y){
			UniformGridCell& cell = cells[cellId(x, y)];
			--cell.nodeCount;
		}
	}
	// insert last node in array instead of this one and update link to that node
	if(nodes.deallocate(id)){
		data = node.data;
		newId = id;
		return true;
	}
	return false;
}
void UniformGrid::update(int id, Vec& translation){
	AABB& aabb = nodes[id].aabb;
	int oldMinX = (int)floor(aabb.min.x/width);
	int oldMinY = (int)floor(aabb.min.y/height);
	int oldMaxX = (int)floor(aabb.max.x/width);
	int oldMaxY = (int)floor(aabb.max.y/height);
	aabb.translate(translation);
	int minX = (int)floor(aabb.min.x/width);
	int minY = (int)floor(aabb.min.y/height);
	int maxX = (int)floor(aabb.max.x/width);
	int maxY = (int)floor(aabb.max.y/height);
	// if the node doesnt stay in the same cells
	if(oldMinX != minX || oldMinY != minY || oldMaxX != maxX || oldMaxY != maxY){
		// decrease nodeCount in cells this node ceases to overlap with
		for(int x = oldMinX; x <= oldMaxX; ++x){
			for(int y = oldMinY; y <= oldMaxY; ++y){
				UniformGridCell& cell = cells[cellId(x, y)];
				--cell.nodeCount;
			}
		}
		// increase nodeCount in cells this node starts to overlap with
		for(int x = minX; x <= maxX; ++x){
			for(int y = minY; y <= maxY; ++y){
				UniformGridCell& cell = cells[cellId(x, y)];
				++cell.nodeCount;
			}
		}
	}
}
void UniformGrid::getCollisions(){
	collisions.clear();
	// clear all cells and provide memory for nodes they contain
	uint free = 0;
	for(ushort i = 0; i < cellsX*cellsY; ++i){
		UniformGridCell& cell = cells[i];
		cell.insertedNodes = 0;
		cell.nodeList = free;
		free += cell.nodeCount;
	}
	// ensure that nodesInCells has enough memory for all entries
	if(nodesInCellsCapacity < free){
		do{
			nodesInCellsCapacity <<= 1;
		}while(nodesInCellsCapacity < free);
		delete[] nodesInCells;
		nodesInCells = new ushort[nodesInCellsCapacity];
	}
	// already checked nodes will be stamped to prevent reporting collision multiple times
	// go through all nodes
	for(ushort i = 0; i < nodes.count; ++i){
		UniformGridNode& node = nodes[i];
		node.stamp = i;
		int minX = (int)floor(node.aabb.min.x/width);
		int minY = (int)floor(node.aabb.min.y/height);
		int maxX = (int)floor(node.aabb.max.x/width);
		int maxY = (int)floor(node.aabb.max.y/height);
		// for each node go through all cells it is in
		for(int x = minX; x <= maxX; ++x){
			for(int y = minY; y <= maxY; ++y){
				UniformGridCell& cell = cells[cellId(x, y)];
				// for each cell go through all nodes it contains
				for(uint k = cell.nodeList; k < cell.nodeList + cell.insertedNodes; ++k){
					ushort otherId = nodesInCells[k];
					UniformGridNode& other = nodes[otherId];
					// if this is the current node or if this node has already been checked with current node continue
					if(other.stamp == i){
						continue;
					}
					// mark this node as checked
					other.stamp = i;
					// if nodes are colliding report collisions
					if(areColliding(node.aabb, other.aabb)){
						collisions.push(Pair<int>(other.data, node.data));
					}
				}
				// add node to cell
				nodesInCells[cell.nodeList + cell.insertedNodes] = i;
				++cell.insertedNodes;
			}
		}
	}
}

void UniformGrid::print(){
	std::cout << "\n\nUniform grid:\n";
	std::cout << "Grid size: " << cellsX << " x " << cellsY << " = " << (cellsX*cellsY) << " cells." << "\n";
	std::cout << "Cell size: " << width << " x " << height << "\n";
	int nonempty = 0;
	int nodeCount = 0;
	for(ushort i = 0; i < cellsX*cellsY; ++i){
		UniformGridCell& cell = cells[i];
		if(cell.nodeCount != 0){
			nodeCount += cell.nodeCount;
			++nonempty;
		}
	}
	std::cout << "Nonempty cells: " << nonempty << "\n";
	std::cout << "Average nodes per cell: " << (float)nodeCount/(float)nonempty << "\n";
}
