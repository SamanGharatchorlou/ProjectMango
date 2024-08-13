#pragma once

class Window;
class InputManager;
class RenderManager;
class UIManager;
class FrameRateController;
class ConfigManager;
class AudioManager;
struct SystemStateManager;
namespace ECS { struct EntityCoordinator; }

struct GameData
{
	static GameData& Get();

	void setup();

	void init(Window* newWindow);
	void preLoad();
	void load();
	void setupObservers();
	void free();

	bool endLoading();

	Window* window = nullptr;

	ConfigManager* configs = nullptr;
	InputManager* inputManager = nullptr;
	RenderManager* renderManager = nullptr;
	UIManager* uiManager = nullptr;
	AudioManager* audioManager = nullptr;
	SystemStateManager* systemStateManager = nullptr;
	ECS::EntityCoordinator* ecs = nullptr;
};
