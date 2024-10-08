#pragma once


class PerformanceProfiler
{
public:
	PerformanceProfiler();

	void start() { mTimer.Start(); }
	void restart() { mTimer.Restart(); }

	void displayTimeSeconds();
	void displayTimeMilliSeconds();

	void displayAverageTimeSeconds();
	void displayAverageTimeMilliSeconds();
	void displayAverageTimeEvery(float seconds);

	void clearAverageTimeEvery(float seconds) { averageResetTime = seconds; }
	void saveToAverage();

public:
	BasicString mName;


private:
	TimerF mTimer;

	float mTotalTime;
	int mCount;

	TimerF mDisplayTimer;

	float averageResetTime;
	TimerF averageResetTimer;
};