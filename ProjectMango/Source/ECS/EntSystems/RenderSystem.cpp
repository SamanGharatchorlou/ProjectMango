#include "pch.h"
#include "RenderSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Game/Camera/Camera.h"
#include "Graphics/RenderManager.h"


namespace ECS
{
	void RenderSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		RenderManager* renderer = GameData::Get().renderManager;

		// increase the camera size so we draw a little extra than the actual screen
		// if we're moving (especially fast) we might get white bars where since the 
		// order of move the camera/player and these updates can be anything
		RectF camera_rect = Camera::Get()->GetRect();
		VectorF center = camera_rect.Center();
		camera_rect.SetSize(camera_rect.Size() * 1.1f);
		camera_rect.SetCenter(center);
		
 		for (Entity entity : entities)
		{
			const Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
			if(!sprite.texture)
				continue;
			
			const Transform& transform = ecs->GetComponentRef(Transform, entity);
			const RectF render_rect(transform.worldPosition + transform.renderOffset, transform.size);

			if(!camera_rect.Intersect(render_rect))
				continue;

			RenderPack pack(sprite.texture, render_rect, sprite.renderLayer);
			pack.subRect = sprite.subRect;
			pack.flip = sprite.flip;
			pack.flipPoint = sprite.flipPoint * render_rect.Size();

			renderer->AddRenderPacket(pack);
		}
	}
}