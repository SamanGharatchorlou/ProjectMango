#include "pch.h"
#include "MainMenuScreen.h"
#include "Game/SystemStateManager.h"
#include "UI/UIManager.h"


void MainMenuScreen::Init()
{
	//GameData::Get().uiManager->controller()->openPopup("IntroductionPopup");
};

void MainMenuScreen::Update()
{
	// todo: hack skip right into game state
	if (released("PlayButton") || true)
	{
		u32 flags = 0;
		SetFlag<u32>(flags, Screen::RenderBelow);

		//GameData::Get().uiManager->controller()->addScreen(UIScreen::Type::Settings, flags);
		GameData::Get().systemStateManager->addState(SystemStates::GameState);
	}
	else if (released("ExitButton"))
	{
		GameData::Get().systemStateManager->quit();
	}
}


void MainMenuScreen::Restart()
{
	GameData::Get().systemStateManager->replaceState(SystemStates::GameState);
}