#pragma once

#include "Screen.h"

class LoadingScreen : public Screen
{
public:
	LoadingScreen() { }

	void Init() override { }
	void HandleInput(InputManager* input) { };
	void Update() override { };
	void Exit() override { }

	UIScreen::Type type() override { return UIScreen::Type::LoadingScreen; }
};