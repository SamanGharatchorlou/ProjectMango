#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/GraphicComponents.h"
#include "ECS/EntSystems/RenderSystem.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "Graphics/TextureManager.h"
#include "imgui-master/imgui.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "Graphics/STexture.h"

namespace DebugMenu
{
	using namespace ECS;

	static bool s_flipOverride = false;
	static SDL_RendererFlip s_spriteFlip = SDL_FLIP_NONE;

	u32 DoSpriteDebugMenu(Entity& entity)
	{
		COMPONENT_PREAMBLE(Sprite);

		ImGui::PushID(entity + type_id);
		if (ImGui::CollapsingHeader(type_name.c_str()))
		{
			Sprite& sprite = GetComponentRef(Sprite, entity);

			StringBuffer64 name;
			if (sprite.image.texture)
			{
				name = TextureManager::Get()->getTextureName(sprite.image.texture);
			}
			if (ImGui::SpriteSheetCombo(name))
			{
				sprite.image.id = name.c_str();
				sprite.SetTexture(sprite.image.id.c_str());
			}

			int layer = (int)sprite.params.renderLayer;
			if (ImGui::SliderInt("Render Layer", &layer, 0, (int)RenderLayer::Top))
			{
				sprite.params.renderLayer = (RenderLayer)layer;
			}

			ImGui::Text(sprite.params.flip == SDL_FLIP_HORIZONTAL ? "No flip" : "Horizontal flip");
			
			ImGui::SameLine();
			ImGui::Checkbox("Flip override", &s_flipOverride);

			ImGui::SameLine();
			if (ImGui::Button("Flip sprite"))
			{
				s_flipOverride = true;

				if (s_spriteFlip == SDL_FLIP_HORIZONTAL)
					s_spriteFlip = SDL_FLIP_NONE;
				else
					s_spriteFlip = SDL_FLIP_HORIZONTAL;
			}

			// messing around with the flip here... could break it
			if (s_flipOverride)
				sprite.params.flip = s_spriteFlip; 

			ImGui::Text("rotation: %f", sprite.params.rotation);

			ImGui::Text("render offset %f, %f", sprite.params.renderOffset.x, sprite.params.renderOffset.y);
		}

		ImGui::PopID();

		return type_id;
	}


	u32 DoSpriteSheetDebugMenu(Entity& entity)
	{
		COMPONENT_PREAMBLE(SpriteSheet);

		ImGui::PushID(entity + type_id);
		if (ImGui::CollapsingHeader(type_name.c_str()))
		{
			SpriteSheet& ss = GetComponentRef(SpriteSheet, entity);

			if(ImGui::InputVectorI("Grid Count", ss.frame.gridCount))
			{
				Sprite* sprite = GetComponent(Sprite, entity);
				if (sprite && sprite->image.texture)
					ss.frame.frameSize = sprite->image.texture->originalDimentions / ss.frame.gridCount.toFloat();
			}

			ImGui::InputInt("Starting Index", &ss.index, 0, ss.frame.gridCount.length() - 1);
		}

		ImGui::PopID();

		return type_id;
	}
}