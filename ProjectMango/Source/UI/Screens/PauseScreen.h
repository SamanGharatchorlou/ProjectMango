#pragma once

#include "Screen.h"

class PauseScreen : public Screen
{
public:
	enum Option
	{
		None,
		Resume,
		Settings,
		Restart,
		Quit
	};

public:
	PauseScreen() { }

	void Init() override { }
	void HandleInput(InputManager* input);
	void Update() override;
	void Exit() override { };

	UIScreen::Type type() override { return UIScreen::Type::Pause; }

private:
	void popScreenState();

};