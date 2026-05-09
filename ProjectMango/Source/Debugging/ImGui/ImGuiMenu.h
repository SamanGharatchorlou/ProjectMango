#pragma once

#define IMGUI 1

#if IMGUI
namespace DebugMenu
{
	struct DebugState
	{
		bool entitySystemWindow = false;
		bool partViewerWindow = false;
		bool inputWindow = false;
		bool transformWindow = false;
		bool gameStateWindow = false;
		bool tweakerWindow = false;
		bool editorWindow = false;
		bool demoWindow = false;

		static DebugState& Get();
	};

	void Init();
	bool HandleInput(SDL_Event& event);
	void Draw();

	void OpenEditorWindow();

	void ToggleShow();
}
#endif