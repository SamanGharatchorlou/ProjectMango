#include "pch.h"
#include "GameController.h"

#include "Graphics/Renderer.h"
#include "Game/GameSetup.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"

// GameData
#include "Audio/AudioManager.h"
#include "Input/inputManager.h"
#include "Graphics/RenderManager.h"
#include "UI/UIManager.h"
#include "Game/FrameRateController.h"
#include "Game/States/StartupState.h"


GameController::GameController()
{
	StartupState* startup_state = new StartupState(mGameData);

	SystemStateManager* sm = mGameData.systemStateManager;
	sm->mStates.addState(startup_state);
}

void GameController::run()
{
	SystemStateManager* sm = mGameData.systemStateManager;

	// add first game state
	//sm->replaceState(SystemStates::GameState);
    sm->mStates.replaceState(new GameState);

	FrameRateController& frc = FrameRateController::Get();
	frc.start();

	SDL_Event event;

	// main game loop
	while (!sm->mQuit)
	{
#if FRAMERATE_CAP
		frc.resetCapTimer();
#endif

		handleInput(event);
		updateLoops(frc.delta());
		render();

		frc.update();

		if (sm->mRestart)
		{
			restartGame();
		}
	}

	DebugPrint(Log, "Game Ended");
}

void GameController::free()
{
	mGameData.free();

	// delete globals
	FileManager::Get()->free();
	Renderer::Get()->free();

	GameSetup::closeSubSystems();

	DebugPrint(Log, "GameController destroyed");
}


void GameController::restartGame()
{
	SystemStateManager* sm = mGameData.systemStateManager;
	sm->mRestart = false;
	DebugPrint(Log, "--- Begin game restart ---");

	GameSetup::setTutorial("OFF");
	AudioManager::Get()->resetController();

	// Remove all states
	sm->mStates.shallowClear();

	mGameData.uiManager->clearScreens();
	mGameData.uiManager->load();

	//mGameData.environment->init(&mGameData);
	//mGameData.environment->load();
	//sm->addState(SystemStates::PreGameState);
	// todo: clear / pop whatever state is on here
    sm->mStates.addState(new GameState);
	DebugPrint(Log, "--- End game restart ---");
}


// --- Private Functions --- //

void GameController::handleInput(SDL_Event& event)
{
	SystemStateManager* sm = mGameData.systemStateManager;

	// handle input events
	mGameData.inputManager->consumeBufferedInputs();
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			sm->mQuit = true;

		mGameData.inputManager->processInputEvent(event);
	}
	mGameData.inputManager->updateHeldFrame();

	mGameData.uiManager->handleInput(mGameData.inputManager);
	sm->mStates.getActiveState().HandleInput();

#if DEBUG_CURSOR // show mouse position in screen title
	VectorF pos = mGameData.inputManager->cursorPosition();
	const BasicString cursor = "X: " + BasicString(pos.x) + " Y: " + BasicString(pos.y);
	mGameData.window->setTitle(cursor.c_str());
#endif
}

void GameController::updateLoops(float dt)
{
	SystemStateManager* sm = mGameData.systemStateManager;

	AudioManager::Get()->Update();

	// Fast update runs updateLoopRepeats number of times per frame
	float updateLoopRepeats = 10;
	for (int i = 0; i < updateLoopRepeats; i++)
		sm->mStates.getActiveState().FastUpdate(dt / updateLoopRepeats);

	sm->mStates.getActiveState().Update(dt);

	mGameData.uiManager->update();
}


void GameController::render()
{
	mGameData.renderManager->render();
}

