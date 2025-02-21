#include "pch.h"
#include "ComponentDebugMenu.h"

#include "imgui-master/imgui.h"

#include "ECS/EntityCoordinator.h"
#include "Graphics/TextureManager.h"
#include "Game/FrameRateController.h"
#include "ECS/Components/Animator.h"
#include "Animations/CharacterStates.h"
#include "Game/FrameRateController.h"

struct RenderRects
{
	bool render = true;
	bool object = true;
	bool collider = true;
};

static RenderRects s_renderRects;
static bool s_flipOverride = false;
static SDL_RendererFlip s_spriteFlip = SDL_FLIP_NONE;
static ActionState s_activeAction = ActionState::None;

std::vector<BasicString> s_animationLog;

ECS::Component::Type DebugMenu::DoAnimatorDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Animator;

	if (!HasComponent(Transform, entity))
		return type;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		const ECS::Animator& animator = GetComponentRef(Animator, entity);

		ActionState active_animation = animator.GetActiveAnimation().action;
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

	return type;
}


ECS::Component::Type DebugMenu::DoSpriteDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Sprite;

	if (!HasComponent(Transform, entity))
		return type;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		const ECS::Sprite& sprite = GetComponentRef(Sprite, entity);
			
		if (ImGui::TreeNode("Component Data"))
		{
			StringBuffer64 spriteName = TextureManager::Get()->getTextureName(sprite.texture);
			if (spriteName.empty())
				spriteName = "No sprite";
			ImGui::Text("SpriteSheet: %s", spriteName.c_str());

			ImGui::Text(sprite.flip == SDL_FLIP_HORIZONTAL ? "No flip" : "Horizontal flip");

			ImGui::Checkbox("Flip override", &s_flipOverride);

			if(ImGui::Button("Flip Sprite"))
			{
				s_flipOverride = true;

				if(s_spriteFlip == SDL_FLIP_HORIZONTAL)
					s_spriteFlip = SDL_FLIP_NONE;
				else
					s_spriteFlip = SDL_FLIP_HORIZONTAL;
			}

			ImGui::TreePop();
		}

		if (ImGui::TreeNode("Display"))
		{
			ECS::Transform& transform = GetComponentRef(Transform, entity);
			ECS::Sprite& sprite = GetComponentRef(Sprite, entity);

			ImGui::Checkbox("Render Rect", &s_renderRects.render);
			if(s_renderRects.render)
			{	
				const RectF renderRect(transform.worldPosition, transform.size);
				DebugDraw::RectOutline(renderRect, Colour::Green);
			}

			ImGui::TreePop();
		}
	}

	ImGui::PopID();

	return type;
}