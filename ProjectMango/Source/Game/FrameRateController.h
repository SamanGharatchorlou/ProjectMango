#pragma once

class FrameRateController
{
public:
	static FrameRateController& Get();

	float delta() const { return (1 / 120.0f); }
	void start();
	void update();

	int FrameCount() const { return frameCount; }
	float GameSeconds() const { return gameTimer.GetSeconds(); }
	
	TimerF frameTimer;
	float dt = 0.0f;

	TimerF capTimer;
	int frameRateCap = 0; 

	TimerF gameTimer;
	int frameCount = 0;
};
