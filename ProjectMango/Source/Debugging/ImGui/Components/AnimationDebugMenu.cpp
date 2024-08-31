#include "pch.h"
#include "ComponentDebugMenu.h"

#include "Debugging/ImGui/ImGuiHelpers.h"
#include "imgui-master/imgui.h"

#include "ECS/EntityCoordinator.h"
#include "Graphics/TextureManager.h"
#include "Game/FrameRateController.h"
#include "ECS/Components/ComponentCommon.h"
#include "ECS/Components/Animator.h"
#include "Animations/CharacterStates.h"
#include "Game/FrameRateController.h"
//#include "Core/BasicString.h"

//static std::vector<ActionState> s_animationPlaylist;
//static std::vector<Animation> s_animationPlaylist;
static int s_playlistIndex = 0;
static bool s_usingPlaylist = false;

static bool s_playingPlaylist = false;
static bool s_playSingleAnimation = false;
static bool s_displayRenderRect = false;
static bool s_forceLooping = true;

struct RenderRects
{
	bool render = true;
	bool object = true;
	bool collider = true;
};

static RenderRects s_renderRects;

static bool s_flipOverride = false;
static SDL_RendererFlip s_spriteFlip = SDL_FLIP_NONE;

static ECS::Entity s_activeEnt;

bool DebugMenu::UsingPlaylist(ECS::Entity& entity) { return s_usingPlaylist && s_activeEnt == entity; }
bool DebugMenu::DisplayRenderRect(ECS::Entity& entity) { return s_displayRenderRect && s_activeEnt == entity; }
bool DebugMenu::SpriteFlipOverride(ECS::Entity& entity, SDL_RendererFlip& sprite_flip) 
{ 
	if(s_flipOverride && s_activeEnt == entity)
	{
		sprite_flip = s_spriteFlip;
		return true;
	}

	return false;
}

VectorI facing_direction;

ActionState s_activeAction = ActionState::None;

std::vector<BasicString> s_animationLog;

ECS::Component::Type DebugMenu::DoAnimatorDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Animator;

	if (!ecs->HasComponent(Transform, entity))
		return type;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		const ECS::Animator& animator = ecs->GetComponentRef(Animator, entity);

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
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Sprite;

	if (!ecs->HasComponent(Transform, entity))
		return type;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		const ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
			
		if (ImGui::TreeNode("Component Data"))
		{
			StringBuffer64 spriteName = TextureManager::Get()->getTextureName(sprite.texture);
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
			ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
			ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);


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