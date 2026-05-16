#include "pch.h"
#include "ComponentDebugMenu.h"

#include "imgui-master/imgui.h"

#include "ECS/EntityCoordinator.h"
#include "Graphics/TextureManager.h"
#include "Game/FrameRateController.h"
#include "ECS/Components/IncludeComponents.h"
#include "Graphics/RenderManager.h"
#include "ECS/EntSystems/RenderSystem.h"

struct RenderRects
{
	bool render = true;
	bool object = true;
	bool collider = true;
};

static RenderRects s_renderRects;
static bool s_flipOverride = false;
static SDL_RendererFlip s_spriteFlip = SDL_FLIP_NONE;
static Action::Enum s_activeAction = Action::None;

std::vector<BasicString> s_animationLog;

u32 DebugMenu::DoAnimatorDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Animator::TypeName();
	ComponentID type_id = Animator::TypeId();

	if (!HasComponent(Transform, entity))
		return type_id;

	ImGui::PushID(entity + type_id);
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		const ECS::Animator& animator = GetComponentRef(Animator, entity);

		Action::Enum active_animation = animator.GetActiveAnimation()->action;
		if(active_animation != s_activeAction)
		{
			s_activeAction = active_animation;

			const FrameRateController& frc = FrameRateController::Get();
			char new_string_log[128];

			const char* action = ActionToString(active_animation);
			float start_time = frc.GameSeconds();

			snprintf(new_string_log, 128, "Action: %s | Time: %f", action, start_time);
			s_animationLog.push_back(new_string_log);
		}

		for( u32 i = 0; i < s_animationLog.size(); i++ )
		{
			ImGui::Text(s_animationLog[i].c_str());
		}

		ImGui::Text("Loops: %d", animator.loopCount);
	}


	ImGui::PopID();

	return type_id;
}

u32 DebugMenu::DoSpriteDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Sprite::TypeName();
	ComponentID type_id = Sprite::TypeId();

	if (!HasComponent(Transform, entity))
		return type_id;

	ImGui::PushID(entity + type_id);
	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Sprite& sprite = GetComponentRef(Sprite, entity);
			
		if (ImGui::TreeNode("Component Data"))
		{
			StringBuffer64 spriteName = TextureManager::Get()->getTextureName(sprite.image.texture);
			if (spriteName.empty())
				spriteName = "No sprite";
			ImGui::Text("Sprite: %s", spriteName.c_str());

			ImGui::Text(sprite.params.flip == SDL_FLIP_HORIZONTAL ? "No flip" : "Horizontal flip");

			ImGui::Checkbox("Flip override", &s_flipOverride);

			if(ImGui::Button("Flip Sprite"))
			{
				s_flipOverride = true;

				if(s_spriteFlip == SDL_FLIP_HORIZONTAL)
					s_spriteFlip = SDL_FLIP_NONE;
				else
					s_spriteFlip = SDL_FLIP_HORIZONTAL;
			}

			// messing around with the flip here... could break it
			if(s_flipOverride)
			{
				sprite.params.flip = s_spriteFlip;
			}
			
			ImGui::Text("rotation: %f", sprite.params.rotation);

			ImGui::Text("render offset %f, %f", sprite.params.renderOffset.x,sprite.params.renderOffset.y);

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Display"))
		{
			ECS::Transform& transform = GetComponentRef(Transform, entity);

			ImGui::Checkbox("Render Rect", &s_renderRects.render);
			if(s_renderRects.render)
			{
				RenderPack pack;
				ECS::GenerateRenderPack(sprite, pack);

				DebugDraw::RectOutline(pack.rect, SColour::Green);

				VectorF about_point = (pack.flipPoint) + pack.rect.TopLeft();

				QuadF quad(pack.rect);
				quad.rotate(pack.rotation, about_point);

				DebugDraw::Quad(quad, SColour::Purple);
				DebugDraw::Point(about_point, SColour::Red, 2.0f);
			}

			ImGui::TreePop();
		}
	}

	ImGui::PopID();

	return type_id;
}