#ifndef __MKBD_TIMER_HPP__
#define __MKBD_TIMER_HPP__

#include <chrono>

class Timer {
	using Clock = std::chrono::high_resolution_clock;

	Clock::time_point mStartTime;

	bool mIsPaused = true;
	double mPauseStartTime = 0;
	double mPauseDuration = 0;
	double mLapStartTime = 0;

public:
	Timer(void) { setClockStartTime(); };

	double now(void);

	double start(void) { return unpause(); };
	double stop(void) { return pause(); };
	double unpause(void);
	double pause(void);

	void reset(void);

	void skip(double duration);
	void set(double time);

	bool isPaused(void) { return mIsPaused; };

	double lap(void);
	double getLap(void);
	void resetLap(void);

private:
	void setClockStartTime(void);
	double getClockTime(void);
};

#endif