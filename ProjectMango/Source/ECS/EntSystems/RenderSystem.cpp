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

 		for (Entity entity : entities)
		{
			Transform& transform = ecs->GetComponentRef(Transform, entity);
			Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
			if(!sprite.texture)
				continue;

			const RectF renderRect(transform.worldPosition, transform.size);

			RenderPack pack(sprite.texture, renderRect, sprite.renderLayer);
			pack.subRect = sprite.subRect;
			pack.flip = sprite.flip;
			pack.flipPoint = sprite.flipPoint * renderRect.Size();

			renderer->AddRenderPacket(pack);
		}
	}
}