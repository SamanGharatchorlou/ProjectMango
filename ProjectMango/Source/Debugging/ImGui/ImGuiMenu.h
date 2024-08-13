#pragma once

#define IMGUI 1

#if IMGUI
namespace DebugMenu
{
	void Init();
	bool HandleInput(SDL_Event& event);
	void Draw();
}
#endif