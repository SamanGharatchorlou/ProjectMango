#pragma once

#include "Screen.h"


class GameOverScreen : public Screen
{
public:
	GameOverScreen() { }

	void Init() override;
	void Update() override;
	void Exit() override { };

	UIScreen::Type type() override { return UIScreen::Type::GameOver; }

	int mMapLevel;
	int mScore;
	int mKills;
	StringBuffer32 mHealth;

private:
	void setTextBox(const char* textbox, int value);
};