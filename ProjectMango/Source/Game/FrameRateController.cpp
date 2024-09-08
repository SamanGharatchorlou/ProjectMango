#include "pch.h"
#include "FrameRateController.h"
#include "System/Files/ConfigManager.h"

static FrameRateController s_frameRateController;

FrameRateController& FrameRateController::Get() { return s_frameRateController; }

void FrameRateController::start()
{
	frameTimer.Start();
	gameTimer.Start();
	
	ConfigManager* cm = ConfigManager::Get();
	GameSettingsConfig* gs = cm->GetConfig<GameSettingsConfig>("GameSettings");
	frameRateCap = gs->settings.getInt("FramerateCap");
	if(frameRateCap > 0)
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
			SDL_Delay((1000 / frameRateCap) - frameTicks);
		}
	}
}
