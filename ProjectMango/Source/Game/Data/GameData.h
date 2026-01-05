#pragma once

class Window;
class InputManager;
class RenderManager;
struct ConfigManager;
class AudioManager;
struct SystemStateManager;
struct UIManager;
struct FontManager;
struct TextureManager;

namespace ECS { struct EntityCoordinator; }

// special case, since we use it everywhere
extern ECS::EntityCoordinator* ecs;

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
	AudioManager* audioManager = nullptr;
	SystemStateManager* systemStateManager = nullptr;
	UIManager* uiManager = nullptr;
	FontManager* fontManager = nullptr;
	TextureManager* textureManager = nullptr;
};

