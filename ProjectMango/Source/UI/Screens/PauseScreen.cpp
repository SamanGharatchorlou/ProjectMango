#include "pch.h"
#include "PauseScreen.h"

#include "UI/ScreenController.h"
#include "Input/InputManager.h"
#include "UI/Elements/UIButton.h"
#include "Game/SystemStateManager.h"
#include "UI/UIManager.h"

void PauseScreen::HandleInput(InputManager* input) 
{
	if (input->isReleased(Button::Pause) || input->isReleased(Button::Esc))
	{
		popScreenState();
	}
	else if (input->isReleased(Button::Esc) || input->isReleased(Button::Quit))
	{
		GameData::Get().systemStateManager->quit();
	}
}


void PauseScreen::Update()
{
	if (released("SettingsButton"))
	{
		GameData::Get().uiManager->controller()->addScreen(UIScreen::Type::Settings);
	}

	if (released("QuitButton"))
	{
		GameData::Get().systemStateManager->quit();
	}
	else if (released("RestartButton"))
	{
		GameData::Get().systemStateManager->restart();
	}
	else if (released("ResumeButton") || released("CloseButton"))
	{
		popScreenState();
	}
}


void PauseScreen::popScreenState()
{
	GameData::Get().uiManager->controller()->popScreen();
	GameData::Get().systemStateManager->popState();
}