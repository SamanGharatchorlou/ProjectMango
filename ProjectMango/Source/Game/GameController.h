#pragma once

#include "Game/Data/GameData.h"
#include "Game/States/State.h"
#include "Game/States/StateMachine.h"
#include "Game/SystemStateManager.h"

class StartupState;
struct ImGuiContext;

class GameController
{
public:
	GameController();

	void run();
	void free();

	ImGuiContext* ctx;

private:
	void handleInput(SDL_Event& event);
	void updateLoops(float dt);
	void render();

	void restartGame();

private:
	GameData mGameData;
	StartupState* mStartupState = nullptr;

#if FRAMERATE_CAP || PRINT_FRAMERATE_EVERY
	Timer<float> fpsTimer;
	Timer<float> capTimer;
#endif
};
