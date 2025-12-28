#include "pch.h"
#include "RenderSystem.h"

#include "ECS/Components/IncludeComponents.h"
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
		pack.texture = sprite.image.texture;
		pack.layer = (u32)sprite.params.renderLayer;
		//pack.subRect = sprite.params.subRect;
		pack.flip = sprite.params.flip;
		pack.rotation = sprite.params.rotation;
		pack.colourMod = sprite.params.colourMod;
	}
		
	//void GenerateRenderPack(const SpriteSheet& sprite_sheet, RenderPack& pack)
	//{
	//	GenerateRenderPack(sprite_sheet.sprite, pack);
	//	pack.subRect = sprite_sheet.frame.GetFrameRect(sprite_sheet.index);
	//}

	void GenerateRenderPack(const UIText& ui_text, RenderPack& pack)
	{
		const Transform& transform = GetComponentRef(Transform, ui_text.entity);
		const RectF render_rect(transform.worldPosition + transform.renderOffset + ui_text.renderOffset, transform.size);

		pack.font = &ui_text.font;
		pack.rect = render_rect;
		pack.layer = (u32)RenderLayer::UI;
	}

	bool IsValid(const RenderPack& pack)
	{
		return (pack.texture || pack.font) && pack.layer > 0;
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
			RectF render_rect(transform.worldPosition + transform.renderOffset, transform.size);
			
			RenderPack pack;
			pack.entity = entity;
			
			if(const Sprite* sprite = GetComponent(Sprite, entity))
			{
				if(camera_rect.Intersect(render_rect))
				{
					pack.rect = render_rect;
					pack.flipPoint = transform.GetHorizontalFlipPoint();
					GenerateRenderPack(*sprite, pack);

					if(const SpriteSheet* sprite_sheet = GetComponent(SpriteSheet, entity))
					{
						if(sprite_sheet->HasValidFrameIndex())
						{
							pack.subRect = sprite_sheet->frame.GetFrameRect(sprite_sheet->index);
						}
					}
					else if(const Animator* animator = GetComponent(Animator, entity))
					{
						if(animator->IsValid())
						{
							const Animation& animation = animator->GetActiveAnimation();
							pack.texture = animation.image.texture;
							pack.subRect = animator->GetActiveSubRect();
						}
					}
				}
			}
			
			// can have a UIText in addition to the other types
			if(const UIText* ui_text = GetComponent(UIText, entity))
			{
				if(!ui_text->text.empty())
				{
					render_rect = render_rect.MoveCopy(ui_text->renderOffset);				
					if(camera_rect.Intersect(render_rect))
					{
						pack.rect = render_rect;
						pack.flipPoint = transform.GetHorizontalFlipPoint();
						pack.font = &ui_text->font;
						pack.layer = (u32)RenderLayer::UI;
					}
				}
			}

			
			if(IsValid(pack))
			{
				renderer->AddRenderPacket(pack);
			}
			
			// can have a LayeredSprite in addition to the other types
			if(const LayeredSprite* layered_sprite = GetComponent(LayeredSprite, entity))
			{
				for( const LayeredSprite::Layer& layer : layered_sprite->spriteLayers)
				{
					if(layer.sprite.IsValid())
					{
						if(!layer.rect.IsZero())
							render_rect = layer.rect;

						if(camera_rect.Intersect(render_rect))
						{
							pack.rect = render_rect;
							pack.flipPoint = transform.GetHorizontalFlipPoint();

							GenerateRenderPack(layer.sprite, pack);

							// we add multiple packs
							renderer->AddRenderPacket(pack);
						}
					}
				}
			}
		}
	}
}