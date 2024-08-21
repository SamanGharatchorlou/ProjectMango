#include "pch.h"
#include "RenderSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "ECS/Components/ComponentCommon.h"
#include "System/Window.h"

#include "Debugging/ImGui/Components/ComponentDebugMenu.h"

// all sizes are relative to a 1024 size screen

namespace ECS
{
	void RenderSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		RenderManager* renderer = GameData::Get().renderManager;

		const VectorF window_size = GameData::Get().window->size();
		const VectorF scale = VectorF(1.0f, 1.0f); //window_size / (VectorF(1024, 1024));


 		for (Entity entity : entities)
		{
			Transform& transform = ecs->GetComponentRef(Transform, entity);
			Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
			if(!sprite.texture)
				continue;

			const RectF renderRect(transform.position, transform.size);

			RenderPack pack(sprite.texture, renderRect, sprite.renderLayer);
			pack.subRect = sprite.subRect;
			pack.flip = sprite.flip;
			pack.flipPoint = sprite.flipPoint * renderRect.Size();

			//float target_flip = renderRect.Height() * 0.5f;

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