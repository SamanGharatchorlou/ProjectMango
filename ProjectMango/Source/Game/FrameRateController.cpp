#include "pch.h"
#include "FrameRateController.h"

static FrameRateController s_frameRateController;

FrameRateController& FrameRateController::Get() { return s_frameRateController; }

FrameRateController::FrameRateController() : dt(0.0f), frameRateCap(0.0f), frameCount(0)
{

}

void FrameRateController::start()
{
	frameTimer.Start();
	gameTimer.Start();

	frameRateCap = FRAMERATE_CAP;
	capTimer.Start();
}


void FrameRateController::update()
{
	dt = frameTimer.GetSeconds();

	frameTimer.Restart();
	frameCount++;

	if(frameRateCap > 0)
		capFrameRate();
}


void FrameRateController::resetCapTimer()
{
	capTimer.Restart();
}

void FrameRateController::capFrameRate()
{
	//If frame finished early
	int frameTicks = (int)capTimer.GetMilliseconds();
	if (frameTicks < (1000 / frameRateCap))
	{
		//Wait remaining time
		SDL_Delay((1000 / (int)frameRateCap) - frameTicks);
	}
}