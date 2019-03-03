#include "Timer.h"

using namespace std;

void Timer::start(){
	startTime = chrono::high_resolution_clock::now();
}
long long Timer::currentTime(){
	auto elapsed = chrono::duration_cast<chrono::nanoseconds>(chrono::high_resolution_clock::now() - startTime);
	return elapsed.count();
}