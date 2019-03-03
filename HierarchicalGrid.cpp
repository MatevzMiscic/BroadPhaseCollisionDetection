#include "HierarchicalGrid.h"
#include <iostream>

void HierarchicalGrid::rehash(int level){
	HierarchicalGridLevel& newLevel = *levels[level];
	// for every node in given level
	for(int i = 0; i < nodes.count; ++i){
		HierarchicalGridNode& node = nodes[i];
		if(node.level == level){
			// free every nodeInCell of this node
			for(int j = 0; j < 4; ++j){
				if(node.indeces[j] != (ushort)-1){
					// we do not have to disconnect it from doubly linked lists because all nodesInCells in given level will be destroyed
					nodesInCells.deallocate(node.indeces[j]);
				}
			}
			float size = firstLevelCellSize * Math::pow(levelToLevel, node.level);
			int minX = (int)floor(node.aabb.min.x/size);
			int minY = (int)floor(node.aabb.min.y/size);
			int maxX = (int)floor(node.aabb.max.x/size);
			int maxY = (int)floor(node.aabb.max.y/size);
			// reinsert node to cells it overlaps in its level
			for(int x = 0; x <= maxX-minX; ++x){
				for(int y = 0; y <= maxY-minY; ++y){
					int nodeInCellId = nodesInCells.allocate();
					HierarchicalGridNodeInCell& nodeInCell = nodesInCells[nodeInCellId];
					nodeInCell.node = i;
					nodeInCell.cell = cellId(minX+x, minY+y);
					ushort next = newLevel.get(nodeInCell.cell);
					if(next != (ushort)-1){
						nodesInCells[next].previous = nodeInCellId;
					}
					newLevel.set(nodeInCell.cell, nodeInCellId);
					++newLevel.count;
					nodeInCell.next = next;
					nodeInCell.previous = (ushort)-1;
					node.indeces[2*x + y] = nodeInCellId;
				}
			}
		}
	}
}

int HierarchicalGrid::add(AABB& aabb, int data){
	int nodeId = nodes.allocate();
	HierarchicalGridNode& node = nodes[nodeId];
	node.aabb = aabb;
	node.data = data;
	node.stamp = nodeId;
	for(int i = 0; i < 4; ++i){
		node.indeces[i] = (ushort)-1;
	}
	// find level in which this node should be inserted
	float minCellSize = nodeToCell * Math::max(aabb.max.x - aabb.min.x, aabb.max.y - aabb.min.y);
	float cellSize = firstLevelCellSize;
	int levelIndex = 0;
	while(cellSize <= minCellSize){
		cellSize *= levelToLevel;
		++levelIndex;
		if(levelIndex == levels.count){
			levels.push(new HierarchicalGridLevel(initialLevelCapacity));
		}
	}
	// find to which cells this node should be added
	node.level = levelIndex;
	HierarchicalGridLevel& level = *levels[levelIndex];
	int minX = (int)floor(aabb.min.x/cellSize);
	int minY = (int)floor(aabb.min.y/cellSize);
	int maxX = (int)floor(aabb.max.x/cellSize);
	int maxY = (int)floor(aabb.max.y/cellSize);
	// add node to cells it overlaps in its level
	for(int x = 0; x <= maxX-minX; ++x){
		for(int y = 0; y <= maxY-minY; ++y){
			if(nodesInCells.free >= nodesInCells.capacity){
				int a = 0;
			}
			int nodeInCellId = nodesInCells.allocate();
			HierarchicalGridNodeInCell& nodeInCell = nodesInCells[nodeInCellId];
			nodeInCell.node = nodeId;
			nodeInCell.cell = cellId(minX+x, minY+y);
			ushort next = level.get(nodeInCell.cell);
			if(next != (ushort)-1){
				nodesInCells[next].previous = nodeInCellId;
			}
			nodeInCell.next = next;
			level.set(nodeInCell.cell, nodeInCellId);
			++level.count;
			nodeInCell.previous = (ushort)-1;
			node.indeces[2*x + y] = nodeInCellId;
		}
	}
	// if level density is too high create bigger level
	if(level.density() > levelMaxDensity){
		int oldCapacity = level.capacity;
		delete &level;
		levels[levelIndex] = new HierarchicalGridLevel(2*oldCapacity);
		rehash(levelIndex);
	}
	return nodeId;
}

bool HierarchicalGrid::remove(int id, int& data, int& newId){
	HierarchicalGridNode& node = nodes[id];
	for(int i = 0; i < 4; ++i){
		if(node.indeces[i] == (ushort)-1){
			continue;
		}
		// remove every nodeInCell of this node from double linked lists
		HierarchicalGridNodeInCell& nodeInCell = nodesInCells[node.indeces[i]];
		if(nodeInCell.previous == (ushort)-1){
			levels[node.level]->set(nodeInCell.cell, nodeInCell.next);
		}else{
			nodesInCells[nodeInCell.previous].next = nodeInCell.next;
		}
		if(nodeInCell.next != (ushort)-1){
			nodesInCells[nodeInCell.next].previous = nodeInCell.previous;
		}
		--levels[node.level]->count;
		// free every nodeInCell of this node
		nodesInCells.deallocate(node.indeces[i]);
	}
	// insert last node in array instead of this one and update link to that node
	if(nodes.deallocate(id)){
		data = node.data;
		newId = id;
		// last node in the array has changed position, so update back pointers to it from its nodesInCell
		for(int i = 0; i < 4; ++i){
			if(node.indeces[i] != (ushort)-1){
				nodesInCells[node.indeces[i]].node = id;
			}
		}
		return true;
	}
	return false;
}

void HierarchicalGrid::update(int id, Vec& translation){
	HierarchicalGridNode& node = nodes[id];
	AABB& aabb = node.aabb;
	float size = firstLevelCellSize * Math::pow(levelToLevel, node.level);
	int oldMinX = (int)floor(aabb.min.x/size);
	int oldMinY = (int)floor(aabb.min.y/size);
	int oldMaxX = (int)floor(aabb.max.x/size);
	int oldMaxY = (int)floor(aabb.max.y/size);
	aabb.translate(translation);
	int minX = (int)floor(aabb.min.x/size);
	int minY = (int)floor(aabb.min.y/size);
	int maxX = (int)floor(aabb.max.x/size);
	int maxY = (int)floor(aabb.max.y/size);
	// if there is a change in what cells node overlaps
	if(oldMinX != minX || oldMinY != minY || oldMaxX != maxX || oldMaxY != maxY){
		// copy indeces to oldIndeces and reset them
		ushort oldIndeces[4];
		for(int i = 0; i < 4; ++i){
			oldIndeces[i] = node.indeces[i];
			node.indeces[i] = (ushort)-1;
		}
		// go through all cells node has overlapped
		for(int x = oldMinX; x <= oldMaxX; ++x){
			for(int y = oldMinY; y <= oldMaxY; ++y){
				// if node no longer overlaps this cell, remove it from the cell
				if((x != minX && x != maxX) || (y != minY && y != maxY)){
					int i = 2*(x-oldMinX) + (y-oldMinY);
					HierarchicalGridNodeInCell& nodeInCell = nodesInCells[oldIndeces[i]];
					if(nodeInCell.previous == (ushort)-1){
						levels[node.level]->set(nodeInCell.cell, nodeInCell.next);
					}else{
						nodesInCells[nodeInCell.previous].next = nodeInCell.next;
					}
					if(nodeInCell.next != (ushort)-1){
						nodesInCells[nodeInCell.next].previous = nodeInCell.previous;
					}
					--levels[node.level]->count;
					nodesInCells.deallocate(oldIndeces[i]);
				}
			}
		}
		// go through all cells node overlaps
		for(int x = minX; x <= maxX; ++x){
			for(int y = minY; y <= maxY; ++y){
				int i = 2*(x-minX) + (y-minY);
				if((x == oldMinX || x == oldMaxX) && (y == oldMinY || y == oldMaxY)){ // if it has already overlapped this cell
					int oldI= 2*(x-oldMinX) + (y-oldMinY);
					node.indeces[i] = oldIndeces[oldI];
				}else{ // if it starts overlapping this cell
					int nodeInCellId = nodesInCells.allocate();
					HierarchicalGridNodeInCell& nodeInCell = nodesInCells[nodeInCellId];
					nodeInCell.node = id;
					nodeInCell.cell = cellId(x, y);
					ushort next = levels[node.level]->get(nodeInCell.cell);
					if(next != (ushort)-1){
						nodesInCells[next].previous = nodeInCellId;
					}
					nodeInCell.next = next;
					levels[node.level]->set(nodeInCell.cell, nodeInCellId);
					++levels[node.level]->count;
					nodeInCell.previous = (ushort)-1;
					node.indeces[i] = nodeInCellId;
				}
			}
		}
	}
}

void HierarchicalGrid::getCollisions(){
	collisions.clear();
	// for every node check collisions with other nodes
	for(int i = 0; i < nodes.count; ++i){
		HierarchicalGridNode& node = nodes[i];
		node.stamp = i;
		float size = firstLevelCellSize * Math::pow(levelToLevel, node.level);
		// go through this level and every higher level
		for(int level = node.level; level < levels.count; ++level){
			int minX = (int)floor(node.aabb.min.x/size);
			int minY = (int)floor(node.aabb.min.y/size);
			int maxX = (int)floor(node.aabb.max.x/size);
			int maxY = (int)floor(node.aabb.max.y/size);
			// for every cell in this level that current node overlaps
			for(int x = 0; x <= maxX-minX; ++x){
				for(int y = 0; y <= maxY-minY; ++y){
					int list = levels[level]->get(cellId(minX+x, minY+y));
					// go through every node in this cell
					while(list != (ushort)-1){
						ushort otherId = nodesInCells[list].node;
						HierarchicalGridNode& other = nodes[otherId];
						list = nodesInCells[list].next;
						// if this is the current node or if this node has already been checked with current node continue
						if(other.stamp == i){
							continue;
						}
						// mark this node as checked
						other.stamp = i;
						// nodes from lower levels report collisions with nodes from higher levels. On the same level node with smaller id reports collisions with node with bigger id.
						if(level > node.level || i < otherId){
							if(areColliding(node.aabb, other.aabb)){
								collisions.push(Pair<int>(node.data, other.data));
							}
						}
					}
				}
			}
			size *= levelToLevel;
		}
	}
}


bool HierarchicalGrid::check2(){
	bool correct = true;
	for(int i = 0; i < nodes.count; ++i){
		HierarchicalGridNode& node = nodes[i];
		for(int j = 0; j < 4; ++j){
			if(node.indeces[j] == (ushort)-1){
				continue;
			}
			HierarchicalGridNodeInCell& nodeInCell = nodesInCells[node.indeces[j]];
			if(nodeInCell.node != i){
				std::cout << "Incorrect reference of node " << i << " to " << j << "\n";
				correct = false;
			}
		}
	}
	return correct;
}
bool HierarchicalGrid::check1(){
	bool correct = true;
	for(int i = 0; i < levels.count; ++i){
		if(i == 13){
			int a = 0;
		}
		HierarchicalGridLevel& level = *levels[i];
		for(int j = 0; j < level.capacity; ++j){
			int index = level.get(j);
			if(index != (ushort)-1 && !nodesInCells.correct(index)){
				std::cout << "level: " << i << " has incorrect reference: " << index << "\n";
				correct = false;
				break;
			}
		}
	}
	return correct;
}
bool HierarchicalGrid::check(){
	// for every node
	bool correct = true;
	float size;
	for(int i = 0; i < nodes.count; ++i){
		if(i == 10){
			int a = 0;
		}
		HierarchicalGridNode& node = nodes[i];
		AABB& aabb = node.aabb;
		size = firstLevelCellSize * Math::pow(levelToLevel, node.level);
		int minX = (int)floor(aabb.min.x/size);
		int minY = (int)floor(aabb.min.y/size);
		int maxX = (int)floor(aabb.max.x/size);
		int maxY = (int)floor(aabb.max.y/size);
		// go through all cells it overlaps
		for(int x = minX; x <= maxX; ++x){
			for(int y = minY; y <= maxY; ++y){
				int list = levels[node.level]->get(cellId(x, y));
				// go through every node in this cell and check if node is present in this cell
				bool present = false;
				while(list != (ushort)-1){
					if(nodesInCells[list].node == i){
						present = true;
						break;
					}
					list = nodesInCells[list].next;
				}
				if(!present){
					correct = false;
					std::cout << "node " << i << " is missing in cell " << x << ", " << y << "\n";
				}
			}
		}
	}
	return correct;
}

void HierarchicalGrid::levelCounts(){
	for(int i = 0; i < levels.count; ++i){
		std::cout << "level " << i+1 << " has " << levels[i]->count << " nodes" << "\n";
	}
}

void HierarchicalGrid::print(){
	std::cout << "\n\nHierarchical grid:\n";
	for(int i = 0; i < levels.count; ++i){
		std::cout << "level " << i+1 << ": " << "nodes: " << levels[i]->count << ", capacity: " << levels[i]->capacity << ", size: " << firstLevelCellSize*Math::pow(levelToLevel, i) << "\n";
	}
	int nonempty = 0;
	int totalNodeCount = 0;
	for(int i = 0; i < levels.count; ++i){
		HierarchicalGridLevel& level = *levels[i];
		// for every bucket
		for(int j = 0; j < level.count; ++j){
			ushort cell = level.cells[j];
			int count = 0;
			while(cell != (ushort)-1){
				++count;
				cell = nodesInCells[cell].next;
			}
			if(count != 0){
				++nonempty;
				totalNodeCount += count;
			}
		}
	}
	std::cout << "Average nodes per bucket: " << (float)totalNodeCount/nonempty << "\n";
	//std::cout << ;
}
