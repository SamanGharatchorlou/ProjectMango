#include "pch.h"
#include "RenderSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "ECS/Components/ComponentCommon.h"

#include "Debugging/ImGui/Components/ComponentDebugMenu.h"

namespace ECS
{
	void RenderSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		RenderManager* renderer = GameData::Get().renderManager;

 		for (Entity entity : entities)
		{
			Transform& transform = ecs->GetComponentRef(Transform, entity);
			Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
			if(!sprite.texture)
				continue;

			const RectF renderRect(transform.position, VectorF(206,66) * 2.0f);

			RenderPack pack(sprite.texture, renderRect, sprite.renderLayer);
			pack.subRect = sprite.subRect;
			pack.flip = sprite.flip;
			pack.flipPoint = ECS::GetObjectRect(entity).Center() - renderRect.TopLeft();

			float target_flip = renderRect.Height() * 0.5f;

			renderer->AddRenderPacket(pack);

			//if( DebugMenu::DisplayRenderRect(entity) )
			//{
			//	//DebugDraw::RectOutline(transform.rect, Colour::Red);
			//	//DebugDraw::RectOutline(pack.rect, Colour::Green);
			//	DebugDraw::Point(pack.rect.TopLeft() + pack.flipPoint, 3.0f, Colour::Blue);
			//}
		}
	}
}