#include <vidd/timer.hpp>

void Timer::setClockStartTime(void) {
	mStartTime = Clock::now();
}

double Timer::getClockTime(void) {
	Clock::time_point clockTime = Clock::now();
	std::chrono::microseconds clockMillTime = std::chrono::duration_cast<std::chrono::microseconds>(clockTime - mStartTime);
	return std::chrono::duration<double>(clockMillTime).count();
}

double Timer::pause(void) {
	if (!mIsPaused) {
		mIsPaused = true;
		mPauseStartTime = getClockTime();
		return now();
	}
	return -1;
}

double Timer::unpause(void) {
	if (mIsPaused) {
		double cTime = getClockTime();
		mIsPaused = false;
		mPauseDuration += cTime - mPauseStartTime;
		return now();
	}
	return -1;
}

double Timer::now(void) {
	if (mIsPaused) {
		return mPauseStartTime - mPauseDuration;
	} else {
		return getClockTime() - mPauseDuration;
	}
}

void Timer::reset(void) {
	setClockStartTime();
	mIsPaused = true;
	mPauseStartTime = 0;
	mPauseDuration = 0;
	mLapStartTime = 0;
}

void Timer::set(double time) {
	mPauseDuration = (now() + mPauseDuration) - time;
}

void Timer::skip(double duration) {
	mPauseDuration -= duration;
}

double Timer::lap(void) {
	double lapTime = getLap();
	resetLap();
	return lapTime;
}

void Timer::resetLap(void) {
	mLapStartTime = getClockTime();
}

double Timer::getLap(void) {
	return now() - mLapStartTime;
}
