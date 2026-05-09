#pragma once

struct RenderPack;

namespace DebugMenu
{
	void DoEntityPartSystemWindow(bool entity_view);
	void DoInputWindow();
	void DoTransformWindow();
	void DoGameStateWindow();
	void DoTweakerWindow();

	u32 GetSelectedEntity();
	void SelectEntity(ECS::Entity entity);

	struct TweakerState
	{
		// entity / component window
		ECS::Entity selectedEntity = -1;
		bool ignoreTerrain = true;
		StringBuffer64 filterBuffer;


		// tweakers
		bool drawRaycasts = false;
		bool canBuyAnyCard = false;

		bool turnLogActive = true;
		std::vector<BasicString> turnLog;


	};

	TweakerState& GetState();

	struct GamePlayerState
	{
		bool isActive = false;
		bool nextFrame = true;
	};

	GamePlayerState& GetGamePlayerState();

	void SendRenderLayerInfo(const std::vector<RenderPack>* render_packs);
}