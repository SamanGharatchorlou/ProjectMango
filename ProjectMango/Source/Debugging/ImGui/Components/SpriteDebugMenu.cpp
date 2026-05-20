#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/GraphicComponents.h"
#include "ECS/EntSystems/RenderSystem.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "Graphics/TextureManager.h"
#include "imgui-master/imgui.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

namespace DebugMenu
{
	using namespace ECS;

	static bool s_flipOverride = false;
	static SDL_RendererFlip s_spriteFlip = SDL_FLIP_NONE;

	u32 DoSpriteDebugMenu(Entity& entity)
	{
		StringBuffer32 type_name = Sprite::TypeName();
		ComponentID type_id = Sprite::TypeId();

		ImGui::PushID(entity + type_id);
		if (ImGui::CollapsingHeader(type_name.c_str()))
		{
			Sprite& sprite = GetComponentRef(Sprite, entity);

			StringBuffer64 name = TextureManager::Get()->getTextureName(sprite.image.texture);
			if (ImGui::SpriteSheetCombo(name))
			{
				sprite.image.texture = TextureManager::Get()->getTexture(name, FileManager::Images);
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


			//if (ImGui::TreeNode("Display"))
			//{
			//	RenderPack pack;
			//	GenerateRenderPack(sprite, pack);

			//	DebugDraw::RectOutline(pack.rect, SColour::Green);

			//	VectorF about_point = (pack.flipPoint) + pack.rect.TopLeft();

			//	QuadF quad(pack.rect);
			//	quad.rotate(pack.rotation, about_point);

			//	DebugDraw::Quad(quad, SColour::Purple);
			//	DebugDraw::Point(about_point, SColour::Red, 2.0f);

			//	ImGui::TreePop();
			//}
		}

		ImGui::PopID();

		return type_id;
	}
}