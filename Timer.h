#pragma once

#include <chrono>

using namespace std;

class Timer{
private:
	chrono::steady_clock::time_point startTime;
public:
	// default constructor
	Timer() = default;

	// starts timer
	void start();

	// returns time that elapsed since the timer was last startes in nanoseconds
	long long currentTime();
};
