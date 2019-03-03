#include "SweepAndPrune.h"
#include <iostream>

void SweepAndPrune::insertionsort(const int lo, const int hi){
	for(int i = lo + 1; i <= hi; ++i){
		for(int j = i; j > lo && compare(pointers[j], pointers[j-1]); --j){
			ushort temp = pointers[j-1];
			pointers[j-1] = pointers[j];
			pointers[j] = temp;
		}
	}
}
int SweepAndPrune::partition(const int lo, const int hi){
	int pivot = pointers[(lo + hi)/2];
	int i = lo - 1;
	int j = hi + 1;
	while(true){
		do{
			++i;
		}while(compare(pointers[i], pivot));
		do{
			--j;
		}while(compare(pivot, pointers[j]));
		if(j <= i){
			return j;
		}
		ushort temp = pointers[i];
		pointers[i] = pointers[j];
		pointers[j] = temp;
	}
}
void SweepAndPrune::quicksort(const int lo, const int hi){
	if(hi - lo < useInsertionSort){
		insertionsort(lo, hi);
	}else{
		int pivot = partition(lo, hi);
		quicksort(lo, pivot);
		quicksort(pivot + 1, hi);
	}
}

void SweepAndPrune::getCollisions(){
	collisions.clear();
	sort();
	for(int i = 0; i < pointers.count; ++i){
		SweepAndPruneNode& nodeA = nodes[pointers[i]];
		AABB& aabbA = nodeA.aabb;
		for(int j = i+1; j < pointers.count; ++j){
			SweepAndPruneNode& nodeB = nodes[pointers[j]];
			AABB& aabbB = nodeB.aabb;
			if(aabbA.max.x < aabbB.min.x){
				break;
			}
			if(areColliding(aabbA, aabbB)){
				collisions.push(Pair<int>(nodeA.data, nodeB.data));
			}
		}
	}
}

void SweepAndPrune::check(){
	for(int i = 0; i < pointers.count; ++i){
		ushort pointer = pointers[i];
		SweepAndPruneNode& node = nodes[pointer];
		std::cout << "pointer: " << pointer << " minX: " << node.aabb.min.x << "\n";
	}
}
