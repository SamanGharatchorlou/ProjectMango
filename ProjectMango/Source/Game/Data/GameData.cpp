#include "pch.h"
#include "GameData.h"

// GameData
#include "System/Window.h"
#include "Graphics/TextureManager.h"
#include "Audio/AudioManager.h"
#include "Input/inputManager.h"
#include "Graphics/RenderManager.h"
#include "Game/Camera/Camera.h"
#include "LoadingManager.h"
#include "Game/SystemStateManager.h"
#include "System/Files/ConfigManager.h"
#include "ECS/EntityCoordinator.h"
#include "UI/UIManager.h"
#include "Graphics/FontManager.h"

#include "Debugging/ImGui/ImGuiMenu.h"

static GameData* gd = nullptr;

ECS::EntityCoordinator* ecs = nullptr;

GameData& GameData::Get()
{
	ASSERT(gd, "Game Data has not been setup yet, cannot call Get until it has been init'd");
	return *gd;
}

void GameData::setup()
{
	gd = this;

	// load the configs on setup, we will likely need its data to setup the rest of the game
	configs = new ConfigManager;
}

void GameData::init(Window* newWindow)
{
	window = newWindow;

	// Set camera before UIManager
	//VectorF size(100.0f, 100.0f);
	//Camera::Get()->setViewport(size);// window->size().toFloat());

	// Rendering
	renderManager = new RenderManager;

	// textures
	textureManager = new TextureManager;
	
	// fonts
	fontManager = new FontManager;

	// game system state
	systemStateManager = new SystemStateManager;

	audioManager = new AudioManager;
	//AudioManager::Get()->init();

	// Input
	inputManager = new InputManager;

	// UI
	uiManager = new UIManager;

	// Entity Component System
	ecs = new ECS::EntityCoordinator;


#if IMGUI
	DebugMenu::Init();
	// default hide
	DebugMenu::ToggleShow();
#endif
}


void GameData::preLoad()
{
	textureManager->preLoad();
	audioManager->preLoad();
}

bool GameData::endLoading()
{
	LoadingManager* loader = LoadingManager::Get();

	if (loader->shouldEarlyExit())
	{
		loader->setLoadingAssets(false);
		return true;
	}

	return false;
}

void GameData::load()
{
	LoadingManager* loader = LoadingManager::Get();
	loader->setLoadingAssets(true);

	if (endLoading())
		return;

	// Texture Manager
	textureManager->load();

	if (endLoading())
		return;

	// Set camera before UIManager
	//VectorF size(100.0f, 100.0f);
	//Camera::Get()->setViewport(size);// window->size().toFloat());

	// Input
	inputManager->init();
	inputManager->setCursorSize(VectorF(25.0f, 25.0f));

	// Audio
	audioManager->load();

	DebugPrint(Log, "finish loading audio");

	if (endLoading())
		return;

	// load this right at the end since some of the above init's might add more configs to load
	configs->ParseAll();

	loader->setLoadingAssets(false);

	DebugPrint(Log, "finish loading");
}


void GameData::setupObservers()
{
	//// Update the current map level
	//environment->addObserver(scoreManager);

	//// Update the UI with all scores
	//scoreManager->addObserver(uiManager);

	//// Update the UI with the players hp and the stats attack, defence etc.
	//environment->actors()->addObserver(environment->actors());
	//environment->actors()->addObserver(uiManager);
	//environment->actors()->addObserver(Camera::Get()->getShake());
	//environment->actors()->addObserver(scoreManager);
	//environment->actors()->addObserver(renderManager);

	//environment->levelManager()->addObserver(environment->actors());
}


void GameData::free()
{
	delete uiManager;
	delete inputManager;
	delete systemStateManager;
	delete ecs;
	delete audioManager;
	delete fontManager;
	delete textureManager;
	delete renderManager;
	delete window;
	delete configs;

	// unlink static getter
	gd = nullptr;

	DebugPrint(Log, "All game data has been deleted");
}
