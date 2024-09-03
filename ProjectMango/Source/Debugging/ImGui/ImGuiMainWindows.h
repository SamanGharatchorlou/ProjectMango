#pragma once

struct RenderPack;

namespace DebugMenu
{
	void DoEntitySystemWindow();
	void DoInputWindow();
	void DoColliderWindow();
	void DoGameStateWindow();
	void DoTweakerWindow();

	u32 GetSelectedEntity();
	void SelectEntity(ECS::Entity entity);

	bool DrawRaycasts();

	struct GamePlayerState
	{
		bool isActive = false;
		bool nextFrame = true;
	};

	GamePlayerState& GetGamePlayerState();

	void SendRenderLayerInfo(const std::vector<RenderPack>* render_packs);
}