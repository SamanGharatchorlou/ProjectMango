#pragma once

#include "Screen.h"

class Texture;

class GameScreen : public Screen
{
public:
	GameScreen();

	void Init() override;
	void HandleInput(InputManager* input);
	void Update() override;
	void Exit() override;

	void Resume() override;
	void Pause() override;

	UIScreen::Type type() override { return UIScreen::Type::Game; }

	void setCursorTexture(Texture* texture);
};