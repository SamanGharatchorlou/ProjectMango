#pragma once


#define FRAMERATE_CAP 60 // Use the FPS cap you want
#define PRINT_FRAMERATE_EVERY 1 // Print at the rate you specify in milliseconds i.e. 500 = print every 0.5 seconds

class FrameRateController
{
public:
	FrameRateController();

	static FrameRateController& Get();

	float delta() const { return dt; }
	void start();
	void update();

	void setFrameCap(float cap) { frameRateCap = cap; }
	void resetCapTimer();
	void capFrameRate();

	int FrameCount() const { return frameCount; }
	float GameSeconds() const { return gameTimer.GetSeconds(); }
	
	// --- debugging --- //
	TimerF frameTimer;
	float dt;

	TimerF capTimer;
	float frameRateCap;

	TimerF gameTimer;
	int frameCount;
};
