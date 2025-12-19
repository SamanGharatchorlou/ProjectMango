#include "pch.h"
#include "RenderSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Game/Camera/Camera.h"
#include "Graphics/RenderManager.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

void SetupSpriteUIBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings);

namespace ECS
{
	std::unordered_map<BasicString, std::function<void(ECS::Entity)>> s_spriteBindings;

	void GenerateRenderPack(const Sprite& sprite, RenderPack& pack)
	{
		pack.texture = sprite.texture;
		pack.layer = (u32)sprite.renderLayer;
		pack.subRect = sprite.subRect;
		pack.flip = sprite.flip;
		pack.flipPoint = sprite.flipPoint * pack.rect.Size();
		pack.rotation = sprite.rotation;
		pack.colourMod = sprite.colourMod;

		pack.entity = sprite.entity;
	}
		
	void GenerateRenderPack(const UIText& ui_text, RenderPack& pack)
	{
		const Transform& transform = GetComponentRef(Transform, ui_text.entity);
		const RectF render_rect(transform.worldPosition + transform.renderOffset + ui_text.renderOffset, transform.size);

		pack.font = &ui_text.font;
		pack.rect = render_rect;
		pack.layer = (u32)RenderLayer::UI;

		pack.entity = ui_text.entity;
	}

	void RenderSystem::Init()
	{
		SetupSpriteUIBindings(s_spriteBindings);
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

			const Transform& transform = GetComponentRef(Transform, entity);
			
			if(const Sprite* sprite = GetComponent(Sprite, entity))
			{
				//if( !sprite->UID.empty() )
				//{
				//	auto iter = s_spriteBindings.find(sprite->UID);
				//	if(iter != s_spriteBindings.end())
				//	{
				//		iter->second(entity);
				//	}
				//}

				if(sprite->texture && sprite->renderLayer != RenderLayer::None && !sprite->disabled)
				{
					const RectF render_rect(transform.worldPosition + transform.renderOffset, transform.size);
					if(camera_rect.Intersect(render_rect))
					{
						RenderPack pack;
						pack.rect = render_rect;
						GenerateRenderPack(*sprite, pack);

						renderer->AddRenderPacket(pack);
					}
				}
			}
			
			if(const UIText* ui_text = GetComponent(UIText, entity))
			{
				if(!ui_text->text.empty())
				{
					const RectF render_rect(transform.worldPosition + transform.renderOffset + ui_text->renderOffset, transform.size);				
					if(camera_rect.Intersect(render_rect))
					{
						RenderPack pack;
						pack.rect = render_rect;
						pack.font = &ui_text->font;
						pack.rect = render_rect;
						pack.layer = (u32)RenderLayer::UI;
						pack.entity = entity;

						renderer->AddRenderPacket(pack);
					}
				}
			}

			if(const LayeredSprite* layered_sprite = GetComponent(LayeredSprite, entity))
			{
				for( const LayeredSprite::Layer& layer : layered_sprite->spriteLayers)
				{
					const Sprite& sprite = layer.sprite;
					if(sprite.texture && sprite.renderLayer != RenderLayer::None && !sprite.disabled)
					{
						RectF render_rect(transform.worldPosition + transform.renderOffset, transform.size);
						if(!layer.rect.IsZero())
							render_rect = layer.rect;

						if(camera_rect.Intersect(render_rect))
						{
							RenderPack pack;
							pack.rect = render_rect;
							GenerateRenderPack(sprite, pack);
							pack.entity = entity;

							renderer->AddRenderPacket(pack);
						}
					}
				}
			}
		}
	}
}