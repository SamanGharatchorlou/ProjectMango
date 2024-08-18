#pragma once

#include "Game/Data/GameData.h"

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

#if FRAMERATE_CAP || PRINT_FRAMERATE_EVERY
	Timer<float> fpsTimer;
	Timer<float> capTimer;
#endif
};
