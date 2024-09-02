#include "pch.h"
#include "StartupState.h"

#include "Game/GameSetup.h"
#include "Game/SystemStateManager.h"
#include "Audio/AudioManager.h"

#include <thread>


StartupState::StartupState(GameData& gd)
{
	GameSetup setup;
	setup.initFileSystem();
	setup.initGameData(gd);

	gd.preLoad();
}

static void loadGameAssets()
{
	GameData& gd = GameData::Get(); 
	gd.load();
}

void renderLoadingBar(LoadingManager* loading)
{
	TimerF timer;
	timer.Start();
	float renderFPS = 20;

	while (loading->isLoadingAssets())
	{
		if (!loading->shouldEarlyExit())
		{
			loading->update();
		}

		// Dont want to hog the renderer too much as its used for loading textures, fonts etc
		if (timer.GetMilliseconds() > (1000 / renderFPS))
		{
			loading->render();
			timer.Restart();
		}
	}
}

void StartupState::Init()
{
	DebugPrint(Log, " -------------------------- starting loader thread -------------------------- ");
	timer.Start();

	AudioManager::Get()->push(AudioEvent(AudioEvent::FadeInMusic, "Menu", nullptr, 1000));

	LoadingManager loader;
	loader.init();

	std::thread assetLoader(loadGameAssets);

	SDL_Event events;

	while (loader.isLoadingAssets())
	{
		while (SDL_PollEvent(&events))
		{
			if (events.type == SDL_QUIT)
			{
				DebugPrint(Warning, "Early exitting loading, closing application");
				loader.earlyExit();
				GameData::Get().systemStateManager->mQuit = true;
				break;
			}
		}
	}

	assetLoader.join();
	//loadingscreen.join();

	loader.exit();
}

void StartupState::Update(float dt)
{

}

void StartupState::Exit()
{
	DebugPrint(Log, "\n\nloading time taken: %fs", timer.GetSeconds());
	DebugPrint(Log, " -------------------------- exiting loader thread -------------------------- ");
}