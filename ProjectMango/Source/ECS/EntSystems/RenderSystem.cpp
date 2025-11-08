#include "pch.h"
#include "RenderSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Game/Camera/Camera.h"
#include "Graphics/RenderManager.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"



namespace ECS
{
	void GenerateRenderPack(const Sprite& sprite, RenderPack& pack)
	{
		const Transform& transform = GetComponentRef(Transform, sprite.entity);
		const RectF render_rect(transform.worldPosition + transform.renderOffset, transform.size);

		pack = RenderPack(sprite.texture, render_rect, (u32)sprite.renderLayer);
		pack.subRect = sprite.subRect;
		pack.flip = sprite.flip;
		pack.flipPoint = sprite.flipPoint * render_rect.Size();
		pack.rotation = sprite.rotation;
		pack.colourMod = sprite.colourMod;

		pack.entity = sprite.entity;
	}

	void RenderSystem::Update(float dt)
	{
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
			// debug break point
			if (DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			const Sprite& sprite = GetComponentRef(Sprite, entity);
			if(!sprite.texture || sprite.renderLayer == RenderLayer::None)
				continue;
			
			const Transform& transform = GetComponentRef(Transform, entity);
			const RectF render_rect(transform.worldPosition + transform.renderOffset, transform.size);

			if(!camera_rect.Intersect(render_rect))
				continue;

			RenderPack pack;
			GenerateRenderPack(sprite, pack);

			renderer->AddRenderPacket(pack);
		}
	}
}