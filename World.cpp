#include "World.h"
#include "Timer.h"

World::World(bool sap, int n, float ratio, float density, float diversity, float destroyRate, float uniformity, float colliderRatio, float velocity): pool(256){
	// set variables
	this->methodCount += (int)sap;
	this->surface = 10000.0f;
	this->colliders = n;
	this->worldRatio = ratio;
	this->density = density;
	this->diversity = diversity;
	this->destroyRate = destroyRate;
	this->uniformity = uniformity;
	this->colliderRatio = colliderRatio;
	this->velocity = velocity;
	this->acceleration = velocity;
	// compute parameters
	sizeX = Math::sqrt(surface*ratio);
	sizeY = sizeX/ratio;
	if(diversity > 1.0001f){
		colliderMinSize = Math::sqrt(4*density*surface*Math::ln(diversity)/colliders/(diversity*diversity-1)/(1+colliderRatio));
	}else{
		colliderMinSize = Math::sqrt(2*density*surface/colliders/(1+colliderRatio));
	}
	colliderMaxSize = colliderMinSize*diversity;
	clusterX[0] = sizeX/3;
	clusterX[1] = sizeX*2/3;
	clusterY[0] = sizeY/3;
	clusterY[1] = sizeY*2/3;
	// make sure values are right
	bool correct = colliderMaxSize*colliderRatio < sizeX && colliderMaxSize*colliderRatio < sizeY;
	// print some values
	/*
	std::cout << "world size: " << sizeX << " x " << sizeY << "\n";
	std::cout << "collider size: from " << colliderMinSize << " to " << colliderMaxSize << "\n";
	//*/
	// create broad phase methods
	method[0] = new BruteForce(); // it doesn't need any parameters
	method[1] = new Tree(); // also no parameters
	setUniformGrid(5, 3); // finds out which cell size gives the best results
	method[3] = new HierarchicalGrid(1.2f*colliderMinSize, 4.0f);
	//setHierarchicalGrid(10); // find out which level to level cell size ratio gives the best results. First level cell size doesn't have much influence on runtime.
	method[4] = new SweepAndPrune(20); // 20 was shown to be the best value no matter the world parameters
	method[5] = new SweepAndPrune(INT_MAX); // only use insertion sort
	// generate world
	for(int i = 0; i < colliders; ++i){
		int colliderId = pool.allocate();
		Collider& collider = pool[colliderId];
		collider.aabb = aabb();
		AABB& box = collider.aabb;
		float size = (box.max.x - box.min.x + box.max.y - box.min.y)/2;
		collider.velocity = vector(velocity*size);
		for(int j = 0; j < methodCount; ++j){
			collider.id[j] = method[j]->add(collider.aabb, colliderId);
		}
	}
}
World::~World(){
	delete method[0];
	delete method[1];
	delete method[2];
	delete method[3];
	delete method[4];
	delete method[5];
}

void World::test(){
	BroadPhase* a = method[0];
	BroadPhase* b = method[1];
	BroadPhase* c = method[2];
	BroadPhase* d = method[3];
	BroadPhase* e = method[4];
	Timer timer;
	long long time;
	bool allCorrect;
	bool valid;
	for(int i = 0; i < 3; ++i){
		allCorrect = true;
		std::cout << "\n" << "Report #" << (i+1) << "\n";
		//print();
		// brute force
		std::cout << "Brute force: " << "\n";
		timer.start();
		a->getCollisions();
		time = timer.currentTime();
		order(a->collisions);
		std::cout << "	Time: " << ((float)time/1000000) << " ms\n";
		// aabb tree
		//*
		std::cout << "Tree: " << "\n";
		timer.start();
		b->getCollisions();
		time = timer.currentTime();
		order(b->collisions);
		valid = validate(a->collisions, b->collisions);
		if(!valid){
			std::cout << "	Error! Printing collisions: " << "\n";
			for(int i = 0; i < b->collisions.count; ++i){
				std::cout << b->collisions[i].a << " and ";
				std::cout << b->collisions[i].b << "\n";
			}
			allCorrect = false;
		}
		std::cout << "	Time: " << ((float)time/1000000) << " ms\n";
		//*/
		// uniform grid
		std::cout << "Uniform grid: " << "\n";
		timer.start();
		c->getCollisions();
		time = timer.currentTime();
		order(c->collisions);
		valid = validate(a->collisions, c->collisions);
		if(!valid){
			std::cout << "	Error! Printing collisions: " << "\n";
			for(int i = 0; i < c->collisions.count; ++i){
				std::cout << c->collisions[i].a << " and ";
				std::cout << c->collisions[i].b << "\n";
			}
			allCorrect = false;
		}
		std::cout << "	Time: " << ((float)time/1000000) << " ms\n";
		// hierarchical grid
		std::cout << "Hierarchical grid: " << "\n";
		timer.start();
		d->getCollisions();
		time = timer.currentTime();
		order(d->collisions);
		valid = validate(a->collisions, d->collisions);
		if(!valid){
			std::cout << "	Error! Printing collisions: " << "\n";
			for(int i = 0; i < d->collisions.count; ++i){
				std::cout << d->collisions[i].a << " and ";
				std::cout << d->collisions[i].b << "\n";
			}
			allCorrect = false;
		}
		std::cout << "	Time: " << ((float)time/1000000) << " ms\n";
		//d.levelCounts();
		//std::cout << "Is everithing correct: " << d.check() << "\n";
		// sweep and prune
		std::cout << "Sweep and prune: " << "\n";
		timer.start();
		e->getCollisions();
		time = timer.currentTime();
		order(e->collisions);
		valid = validate(a->collisions, e->collisions);
		if(!valid){
			std::cout << "	Error! Printing collisions: " << "\n";
			for(int i = 0; i < e->collisions.count; ++i){
				std::cout << e->collisions[i].a << " and ";
				std::cout << e->collisions[i].b << "\n";
			}
			allCorrect = false;
			//e.check();
		}
		std::cout << "	Time: " << ((float)time/1000000) << " ms\n";
		// if there was an error, print collisions reported by brute force
		if(!allCorrect){
			std::cout << "Collisions reported by brute force: " << "\n";
			for(int i = 0; i < a->collisions.count; ++i){
				std::cout << a->collisions[i].a << " and ";
				std::cout << a->collisions[i].b << "\n";
			}
		}
		// print number of collisions
		std::cout << "Number of collisions: " << a->collisions.count;
		((UniformGrid*)c)->print();
		((HierarchicalGrid*)d)->print();
		// update
		run(1);
	}
}

void World::run(int iter){
	for(int it = 0; it < iter; ++it){
		// destroy some colliders
		int killCount = 0;
		int data;
		int newId;
		for(int i = 0; i < pool.count; ++i){
			if(gen.random() <= destroyRate){
				++killCount;
				Collider& collider = pool[i];
				for(int j = 0; j < methodCount; ++j){
					if(method[j]->remove(collider.id[j], data, newId)){
						pool[data].id[j] = newId;
					}
				}
				pool.deallocate(i);
			}
		}
		// create back colliders
		for(int i = 0; i < killCount; ++i){
			int colliderId = pool.allocate();
			Collider& collider = pool[colliderId];
			collider.aabb = aabb();
			AABB& box = collider.aabb;
			float size = (box.max.x - box.min.x + box.max.y - box.min.y)/2;
			collider.velocity = vector(velocity*size);
			for(int j = 0; j < methodCount; ++j){
				collider.id[j] = method[j]->add(collider.aabb, colliderId);
			}
		}
		// update colliders
		Vec min(0, 0);
		Vec max(sizeX, sizeY);
		for(int i = 0; i < pool.count; ++i){
			Collider& collider = pool[i];
			AABB& box = collider.aabb;
			float size = (box.max.x - box.min.x + box.max.y - box.min.y)/2;
			Vec acc = vector(acceleration*size);
			collider.velocity *= 1 - damping;
			collider.velocity += acc;
			Vec minTranlation = min - collider.aabb.min;
			Vec maxTranlation = max - collider.aabb.max;
			Vec translation = Cut(minTranlation, collider.velocity, maxTranlation);
			collider.aabb.translate(translation);
			for(int j = 0; j < methodCount; ++j){
				method[j]->update(collider.id[j], translation);
			}
		}
	}
}

bool World::time(int iter){
	bool valid = true;
	// reset time for each method
	for(int i = 0; i < methodCount; ++i){
		times[i] = 0;
	}
	// time methods
	Timer timer;
	long long dt;
	for(int it = 0; it < iter; ++it){
		run(1);
		for(int i = 0; i < methodCount; ++i){
			timer.start();
			method[i]->getCollisions();
			dt = timer.currentTime();
			times[i] += dt;
			order(method[i]->collisions);
			if(i > 0){
				valid = valid && validate(method[0]->collisions, method[i]->collisions);
			}else{
				/*
				assert(colliders/2 < method[0]->collisions.count && method[0]->collisions.count < colliders*colliders/4, 1);
				if(!(colliders/2 < method[0]->collisions.count && method[0]->collisions.count < colliders*colliders/4)){
					std::cout << "Collision count: " << method[0]->collisions.count << "\n";
				}
				*/
			}
		}
	}
	// make sure right collisions were reported
	if(!valid) std:cout << "Wrong results!";
	return valid;
}

void World::setUniformGrid(int accuracy, int reliability){
	Timer timer;
	float minVal = 0.01f*colliderMaxSize;
	float maxVal = 1.20f*colliderMaxSize;
	float bestVal = maxVal;
	long long bestTime = LLONG_MAX;
	for(float f = 0; f <= 1; f += 1.0f/accuracy){
		float val = minVal + f*(maxVal - minVal);
		long long time = 0;
		for(int i = 0; i < reliability; ++i){
			int cellsX = (int)(sizeX/val);
			int cellsY = (int)(sizeY/val);
			if(cellsX == 0 || UCHAR_MAX < cellsX || cellsY == 0 || UCHAR_MAX < cellsY){
				continue;
			}
			UniformGrid ug(val, val, cellsX, cellsY);
			for(int i = 0; i < Math::min(1000, colliders); ++i){
				AABB box = aabb();
				ug.add(box, i);
			}
			timer.start();
			ug.getCollisions();
			time += timer.currentTime();
		}
		if(time != 0 && time < bestTime){
			bestTime = time;
			bestVal = val;
		}
	}
	//std::cout << "cellSize / colliderMaxSize = " << bestVal/colliderMaxSize << "\n";
	int cellsX = (int)(sizeX/bestVal);
	int cellsY = (int)(sizeY/bestVal);
	/*
	assert(cellsX != 0 && cellsX <= UCHAR_MAX && cellsY != 0 && cellsY <= UCHAR_MAX, 3);
	if(!(cellsX != 0 && cellsX <= UCHAR_MAX && cellsY != 0 && cellsY <= UCHAR_MAX)){
		std::cout << "cellsX: " << cellsX << ", cellsY: " << cellsY << "\n";
	}
	*/
	cellsX = Math::cut(1, cellsX, UCHAR_MAX);
	cellsY = Math::cut(1, cellsY, UCHAR_MAX);
	method[2] = new UniformGrid(bestVal, bestVal, sizeX/bestVal, sizeY/bestVal);
}

void World::setHierarchicalGrid(int reliability){
	Timer timer;
	float minVal = 2.0;
	float maxVal = 4.0;
	float bestVal = maxVal;
	long long bestTime = LLONG_MAX;
	for(int val = minVal; val <= maxVal; ++val){
		long long time = 0;
		for(int i = 0; i < reliability; ++i){
			HierarchicalGrid hg(1.2f*colliderMinSize, val);
			for(int i = 0; i < Math::min(1000, colliders); ++i){
				AABB box = aabb();
				hg.add(box, i);
			}
			timer.start();
			hg.getCollisions();
			time += timer.currentTime();
		}
		if(time < bestTime){
			bestTime = time;
			bestVal = val;
		}
	}
	//std::cout << "levelToLevel = " << bestVal << "\n";
	method[3] = new HierarchicalGrid(1.2f*colliderMinSize, bestVal);
}

void World::setSweepAndPrune(int accuracy, int reliability){
	Timer timer;
	int minVal = 5;
	int maxVal = 60;
	int bestVal = maxVal;
	long long bestTime = LLONG_MAX;
	for(float f = 0; f <= 1; f += 1.0f/accuracy){
		int val = minVal + (int)(f*(maxVal - minVal));
		long long time = 0;
		for(int i = 0; i < reliability; ++i){
			SweepAndPrune sap(val);
			for(int i = 0; i < Math::min(1000, colliders); ++i){
				AABB box = aabb();
				sap.add(box, i);
			}
			timer.start();
			sap.getCollisions();
		
			time += timer.currentTime();
		}
		if(time < bestTime){
			bestTime = time;
			bestVal = val;
		}
	}
	//std::cout << "useInsertionSort = " << bestVal << "\n";
	method[4] = new UniformGrid(bestVal, bestVal, sizeX/bestVal, sizeY/bestVal);
}

AABB World::aabb(){
	float d = gen.randomExp(colliderMinSize, colliderMaxSize);
	float dx = d;
	float dy = d;
	if(gen.random() < 0.5){
		dx *= gen.random(1.0f, colliderRatio);
	}else{
		dy *= gen.random(1.0f, colliderRatio);
	}
	float rx = dx/2;
	float ry = dy/2;
	float x = gen.random(rx, sizeX-rx);
	float y = gen.random(ry, sizeY-ry);
	while(distribution(x, y) < gen.random()){
		x = gen.random(rx, sizeX-rx);
		y = gen.random(ry, sizeY-ry);
	}
	AABB aabb(Vec(x-rx, y-ry), Vec(x+rx, y+ry));
	return aabb;
}
float World::distribution(float x, float y){
	float min = FLT_MAX;
	for(int i = 0; i < points; ++i){
		float dx = clusterX[i] - x;
		float dy = clusterY[i] - y;
		float distance = Math::sqrt(dx*dx + dy*dy);
		min = Math::min(min, distance);
	}
	return Math::pow(uniformity, min);
}


Vec World::vector(float length){
	length *= gen.random();
	float angle = gen.random()*2*Pi;
	return Vec(cos(angle)*length, sin(angle)*length);
}

bool World::compare(const Pair<int>& a, const Pair<int>& b){
	if(a.a < b.a){
		return true;
	}
	if(a.a > b.a){
		return false;
	}
	return a.b < b.b;
}
void World::order(Stack<Pair<int>>& stack){
	// first element in each pair shuld be at least as big as second element
	// switch elements if needed
	for(int i = 0; i < stack.count; ++i){
		Pair<int>& pair = stack[i];
		if(pair.a > pair.b){
			int temp = pair.a;
			pair.a = pair.b;
			pair.b = temp;
		}
	}
	// sort pairs so that the list is ready to be compared with others
	std::sort(stack.array, stack.array + stack.count, compare);
}
bool World::validate(Stack<Pair<int>>& a, Stack<Pair<int>>& b){
	if(a.count != b.count){
		return false;
	}
	for(int i = 0; i < a.count; ++i){
		if(a[i].a != b[i].a || a[i].b != b[i].b){
			return false;
		}
	}
	return true;
}

void World::print(){
	std::cout << "AABBs: " << "\n";
	for(int i = 0; i < pool.count; ++i){
		std::cout << i << ": " << pool[i].aabb.toString() << "\n";
	}
}
