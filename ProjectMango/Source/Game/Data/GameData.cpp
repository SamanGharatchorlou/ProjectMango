#include "pch.h"
#include "GameData.h"

// GameData
#include "System/Window.h"
#include "Graphics/TextureManager.h"
#include "Audio/AudioManager.h"
#include "Input/inputManager.h"
#include "Graphics/RenderManager.h"
#include "UI/UIManager.h"
#include "Game/Camera/Camera.h"
#include "LoadingManager.h"
#include "Game/SystemStateManager.h"
#include "System/Files/ConfigManager.h"
#include "ECS/EntityCoordinator.h"

#include "Debugging/ImGui/ImGuiMenu.h"

static GameData* gd = nullptr;

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
	Camera::Get()->setViewport(window->size().toFloat());

	// Rendering
	renderManager = new RenderManager;

	// game system state
	systemStateManager = new SystemStateManager;

	audioManager = new AudioManager;
	AudioManager::Get()->init();

	// Input
	inputManager = new InputManager;

	// UI
	uiManager = new UIManager;
	uiManager->init();

	// Entity Component System
	ecs = new ECS::EntityCoordinator;

#if IMGUI
	DebugMenu::Init();
#endif
}


void GameData::preLoad()
{
	TextureManager::Get()->preLoad();
	AudioManager::Get()->preLoad();

	uiManager->preLoad();
	uiManager->initCursor(inputManager->getCursor());
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

	DebugPrint(Log, "Start loading textures");

	if (endLoading())
		return;

	// Texture Manager
	TextureManager::Get()->load();

	DebugPrint(Log, "finish loading textures");

	if (endLoading())
		return;

	// Set camera before UIManager
	Camera::Get()->setViewport(window->size().toFloat());

	// Input
	inputManager->init();
	inputManager->setCursorSize(VectorF(25.0f, 25.0f));

	// Audio
	AudioManager::Get()->load();

	DebugPrint(Log, "finish loading audio");

	if (endLoading())
		return;

	// UI
	uiManager->load();
	uiManager->initCursor(inputManager->getCursor());

	DebugPrint(Log, "finish loading UI");

	if (endLoading())
		return;

	// Map Level
	//environment->init(this);
	//environment->load();

	// load this right at the end since some of the above init's might add more configs to load
	configs->load();

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

	AudioManager::Get()->unload();
	TextureManager::Get()->unload();

	delete renderManager;
	delete window;

	// unlink static getter
	gd = nullptr;

	DebugPrint(Log, "All game data has been deleted");
}
