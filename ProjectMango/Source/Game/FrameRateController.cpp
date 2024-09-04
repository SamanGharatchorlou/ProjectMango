#include "pch.h"
#include "FrameRateController.h"

static FrameRateController s_frameRateController;

FrameRateController& FrameRateController::Get() { return s_frameRateController; }

void FrameRateController::start()
{
	frameTimer.Start();
	gameTimer.Start();

	frameRateCap = 60.0f;
	if(frameRateCap > 0.0f)
		capTimer.Start();
}

void FrameRateController::update()
{
	dt = frameTimer.GetSeconds();

	frameTimer.Restart();
	frameCount++;

	if(frameRateCap > 0)
	{
		// if frame finished early
		int frameTicks = (int)capTimer.GetMilliseconds();
		if (frameTicks < (1000 / frameRateCap))
		{
			// wait remaining time
			SDL_Delay((1000 / (int)frameRateCap) - frameTicks);
		}
	}
}
