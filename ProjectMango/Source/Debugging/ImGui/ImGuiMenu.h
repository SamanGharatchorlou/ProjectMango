#pragma once

#define IMGUI 1

#if IMGUI
namespace DebugMenu
{
	u32 GetSelectedEntity();
	void SelectEntity(ECS::Entity entity);

	struct SharedState
	{
		// entity / component window
		ECS::Entity selectedEntity = -1;

		// tweakers
		bool drawRaycasts = false;
		bool canBuyAnyCard = false;

		bool turnLogActive = true;
		std::vector<BasicString> turnLog;

		// camera
		bool displayCameraInfo = false;

		// game state
		bool isActive = false;
		bool nextFrame = true;

	};

	SharedState& GetSharedState();

	void Init();
	bool HandleInput(SDL_Event& event);
	void Draw();

	void ToggleAnimationWindow(bool open);
	void ToggleUIWindow(bool open);

	bool IsAnimationEditorActive();
	bool IsUIEditorActive();

	void ToggleShow();
}
#endif