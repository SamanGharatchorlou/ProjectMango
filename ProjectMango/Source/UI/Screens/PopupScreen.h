#pragma once

#include "Screen.h"

class PopupScreen : public Screen
{
public:
	PopupScreen() { }

	void build(const char* title, const char* body);

	void Init() override { }
	void HandleInput(InputManager* input);
	void Update() override;
	void Exit() override { }

	UIScreen::Type type() { return UIScreen::Type::Popup; }

	void setMainText(const char* text);
private:
	void setTitle(const char* title);
	void setBody(const char* body);

private:
	void exitPopup();

};