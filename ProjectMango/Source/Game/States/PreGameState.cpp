 #include "pch.h"
#include "PreGameState.h"

#include "Audio/AudioManager.h"
#include "UI/UIManager.h"
#include "Graphics/TextureManager.h"
#include "Graphics/Renderer.h"
#include "UI/Screens/MainMenuScreen.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/ComponentManager.h"
#include "ECS/SystemManager.h"

#include "ECS/EntityManager.h"
#include "Graphics/RenderManager.h"
#include "System/Files/ConfigManager.h"

void PreGameState::Init()
{	
	UIManager* UI = GameData::Get().uiManager;

	UI->controller()->clearScreenStack();
	UI->controller()->addScreen(UIScreen::Type::MainMenu);

	if(Texture* cursor_texture = TextureManager::Get()->getTexture("UICursor", FileManager::Image_UI))
		UI->setCursorTexture(cursor_texture);

	AudioManager* audio = AudioManager::Get();
	if(!audio->isPlaying("Menu", nullptr))
		audio->push(AudioEvent(AudioEvent::FadeInMusic, "Menu", nullptr, 1000));
}

void PreGameState::Update(float dt) 
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->UpdateSystems(dt);
}

void PreGameState::Exit()
{
	AudioManager::Get()->push(AudioEvent(AudioEvent::FadeOut, "Menu", nullptr, 150));

	Screen* screen = GameData::Get().uiManager->getActiveScreen();
	MainMenuScreen* mainMenu = dynamic_cast<MainMenuScreen*>(screen);
	if (!mainMenu)
		DebugPrint(Error, "Selection screen is no active, cannot select a character");
}

