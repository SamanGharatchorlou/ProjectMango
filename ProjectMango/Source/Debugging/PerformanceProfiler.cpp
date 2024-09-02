#include "pch.h"
#include "PerformanceProfiler.h"

PerformanceProfiler::PerformanceProfiler() : mTotalTime(0.0f), mCount(0), averageResetTime(-1.0f)
{
	mDisplayTimer.Start();
	averageResetTimer.Start();
}

void PerformanceProfiler::displayAverageTimeEvery(float seconds)
{
	if (mDisplayTimer.GetSeconds() > seconds)
	{
		if(mCount > 0)
			DebugPrint(Profile, "Profiler %s average time %fms", mName.c_str(), mTotalTime / mCount);
		mDisplayTimer.Restart();
	}

	if (averageResetTime > 0.0f && averageResetTimer.GetSeconds() > averageResetTime)
	{
		mTotalTime = 0.0f;
		mCount = 0;
		averageResetTimer.Restart();
	}
}


void PerformanceProfiler::saveToAverage()
{
	mTotalTime += mTimer.GetMilliseconds();
	mCount++;
}


void PerformanceProfiler::displayTimeSeconds()
{
	DebugPrint(Profile, "Profiler %s time %fs", mName.c_str(), mTimer.GetSeconds());
}


void PerformanceProfiler::displayTimeMilliSeconds()
{
	DebugPrint(Profile, "Profiler %s time %fms", mName.c_str(), mTimer.GetMilliseconds());
}


void PerformanceProfiler::displayAverageTimeSeconds()
{
	DebugPrint(Profile, "Profiler %s average time %fs", mName.c_str(), mTotalTime * 0.001f / mCount);
}


void PerformanceProfiler::displayAverageTimeMilliSeconds()
{
	DebugPrint(Profile, "Profiler %s average time %fms", mName.c_str(), mTotalTime / mCount);
}