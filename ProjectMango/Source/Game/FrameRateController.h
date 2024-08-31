#pragma once

// todo: this shouldnt be in debug

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

	unsigned long frameCount() const { return frameNumber; }
	float GameSeconds() const { return gameTimer.getSeconds(); }

#if PRINT_FRAMERATE_EVERY
	void printfFrameRate();
#endif


private:
	TimerF frameTimer;
	float dt;

	TimerF capTimer;
	float frameRateCap;

	TimerF gameTimer;
	unsigned long frameNumber;

	// --- debugging --- //
#if PRINT_FRAMERATE_EVERY
	float fpsSum = 0.0f;
	int fpsCounter = 0;
	float totalFrameTime = 0.0f;
	int totalFrames = 0;
#endif


#if PRINT_FRAMERATE_EVERY
	TimerF fpsTimer;
#endif
};
