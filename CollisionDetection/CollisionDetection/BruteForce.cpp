#include "BruteForce.h"

BruteForce::BruteForce(): nodes(64){

}
BruteForce::~BruteForce(){

}

int BruteForce::add(AABB& aabb, int data){
	int nodeId = nodes.allocate();
	BruteForceNode& node = nodes[nodeId];
	node.aabb = aabb;
	node.data = data;
	return nodeId;
}
bool BruteForce::remove(int id, int& data, int& newId){
	if(nodes.deallocate(id)){
		data = nodes[id].data;
		newId = id;
		return true;
	}
	return false;
}
void BruteForce::update(int id, Vec& translation){
	AABB& aabb = nodes[id].aabb;
	aabb.translate(translation);
}
int BruteForce::countCollisions(){
	int collisionCount = 0;
	for(int i = 0; i < nodes.count; ++i){
		for(int j = i+1; j < nodes.count; ++j){
			if(areColliding(nodes[i].aabb, nodes[j].aabb)){
				++collisionCount;
			}
		}
	}
	return collisionCount;
}
void BruteForce::getCollisions(){
	collisions.clear();
	for(int i = 0; i < nodes.count; ++i){
		for(int j = i+1; j < nodes.count; ++j){
			if(areColliding(nodes[i].aabb, nodes[j].aabb)){
				collisions.push(Pair<int>(nodes[i].data, nodes[j].data));
			}
		}
	}
}