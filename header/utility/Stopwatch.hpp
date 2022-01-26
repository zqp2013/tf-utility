/*
 * Stopwatch.hpp
 *
 *  Created on: 2019年12月3日
 *      Author: qpzhou
 */

#ifndef UTILITY_STOPWATCH_HPP_
#define UTILITY_STOPWATCH_HPP_
#include <iostream>
#include "Macros.h"
#include <list>

#ifdef _WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

namespace utility {

#ifndef _WIN32
/// Compute a platform-specific high-res timer value that fits into an int64.
static int64_t HighResTimer() {
	timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000 * 1000 + tv.tv_usec;
}

///Convert a delta of HighResTimer() values to microseconds.
static int64_t TimerToMicros(int64_t dt) {
	// No conversion necessary.
	return dt;
}
#else
static int64_t LargeIntegerToInt64(const LARGE_INTEGER& i) {
	return ((int64_t)i.HighPart) << 32 | i.LowPart;
}

static int64_t HighResTimer() {
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return LargeIntegerToInt64(counter);
}

static int64_t TimerToMicros(int64_t dt) {
	static int64_t ticks_per_sec = 0;
	if (!ticks_per_sec) {
		LARGE_INTEGER freq;
		QueryPerformanceFrequency(&freq);
		ticks_per_sec = LargeIntegerToInt64(freq);
	}
	// dt is in ticks. We want microseconds.
	return (dt * 1000000) / ticks_per_sec;
}
#endif // _WIN32

class Stopwatch {
public:
	Stopwatch() { Start(); }
	~Stopwatch() {}

	void Start() {
		start_ = Now();
		stop_ = 0;
	}

	void Stop() {
		if (stop_ == 0)
			stop_ = Now();
	}

	double GetElapsedMicroseconds() {
		Stop();
		return double(stop_ - start_);
	}

	double GetElapsedMilliseconds() {
		return GetElapsedMicroseconds() / 1000.;
	}

	double GetElapsedSeconds() {
		return GetElapsedMicroseconds() / 1000000.;
	}

	void Mark(const char* title) {
		points_.emplace_back(std::make_pair(title, double(Now() - start_)));
	}

	void Dump(std::ostream& out) {
		for (auto it = points_.begin(); it != points_.end(); it++)
			out << it->first << ":" << it->second << " micros.";
	}
private:
	static int64_t Now() { return TimerToMicros(HighResTimer()); }
	int64_t start_;
	int64_t stop_;
	std::list<std::pair<std::string, double>> points_;
};

} // end of namespace utility


#endif /* UTILITY_STOPWATCH_HPP_ */
