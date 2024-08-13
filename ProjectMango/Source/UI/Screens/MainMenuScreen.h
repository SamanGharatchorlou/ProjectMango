#pragma once

#include "Screen.h"

class Texture;

class MainMenuScreen : public Screen
{
public:
	MainMenuScreen() { };

	void Init() override;

	void HandleInput(InputManager* input) { };
	void Update() override;

	//void resume() override;
	//void pause() override;
	void Exit() override { };

	UIScreen::Type type() override { return UIScreen::Type::MainMenu; }

	void Restart();

	//void setCursorTexture(Texture* texture);
};