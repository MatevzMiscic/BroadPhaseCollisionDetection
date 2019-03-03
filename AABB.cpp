#include "AABB.h"

AABB::AABB(){

}
AABB::AABB(const Vec& min, const Vec& max){
	this->min = min;
	this->max = max;
}
void AABB::operator= (AABB aabb){
	min = aabb.min;
	max = aabb.max;
}

void AABB::eat(const AABB& aabb){
	min = Min(min, aabb.min);
	max = Max(max, aabb.max);
}
bool AABB::isIn(const AABB& aabb){
	return min.x <= aabb.min.x && max.x >= aabb.max.x && min.y <= aabb.min.y && max.y >= aabb.max.y;
}
void AABB::translate(const Vec& translation){
	min += translation;
	max += translation;
}
void AABB::fatten(float tolerance){
	min.x -= tolerance;
	max.x += tolerance;
	min.y -= tolerance;
	max.y += tolerance;
}
float AABB::area(){
	return (max.x - min.x)*(max.y - min.y);
}

bool areColliding(AABB& a, AABB& b){
	return a.min.x <= b.max.x && b.min.x <= a.max.x && a.min.y <= b.max.y && b.min.y <= a.max.y;
}
AABB eat(AABB& a, AABB& b){
	return AABB(Min(a.min, b.min), Max(a.max, b.max));
}
AABB fatten(AABB& aabb, float fatness){
	Vec fat = Vec(fatness, fatness);
	return AABB(aabb.min - fat, aabb.max + fat);
}
