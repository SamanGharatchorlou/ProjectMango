#pragma once

#include "Screen.h"
#include "UI/Elements/UISlider.h"


class SettingsScreen : public Screen
{
public:
	SettingsScreen() { }

	void Init() override;
	void Update() override;
	void Exit() override { };

	UIScreen::Type type() override { return UIScreen::Type::Settings; }
};