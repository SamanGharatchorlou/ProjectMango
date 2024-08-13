#include "pch.h"
#include "ComponentDebugMenu.h"

#include "Debugging/ImGui/ImGuiHelpers.h"
#include "imgui-master/imgui.h"

#include "ECS/EntityCoordinator.h"
#include "Graphics/TextureManager.h"
#include "Game/FrameRateController.h"
#include "ECS/Components/ComponentCommon.h"

//static std::vector<ActionState> s_animationPlaylist;
static std::vector<Animation> s_animationPlaylist;
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

static Animation* SetAnimation(Animator& animator)
{
	if (s_animationPlaylist.size() > s_playlistIndex)
	{
		Animation& anim = s_animationPlaylist[s_playlistIndex];

		u32 anim_index = animator.getAnimationIndex(anim);
		if(anim_index != -1)
		{
			animator.mAnimationIndex = anim_index;
			return &animator.mAnimations[anim_index];
		}
	}

	if (s_animationPlaylist.size() > 0)
	{
		s_playlistIndex = 0;
		return SetAnimation(animator);
	}

	return nullptr;
}

VectorI facing_direction;

ECS::Component::Type DebugMenu::DoAnimationDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Animation;

	if(entity != s_activeEnt)
	{
		s_animationPlaylist.clear();
		s_playlistIndex = 0;
		s_playingPlaylist = false;
		s_playSingleAnimation = false;
		s_displayRenderRect = false;
		s_forceLooping = true;
	}
	s_activeEnt = entity;

    //if (ecs->HasComponent(entity, type))
    {
		ImGui::PushID(entity + (int)type);
		if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
		{
            ECS::Animation& anim = ecs->GetComponentRef(Animation, entity);
			if (ImGui::TreeNode("Component Data"))
			{
				Animator& animator = anim.animator;
				Animation& animation = animator.mAnimations[animator.mAnimationIndex];

				ImGui::Text("Active Animation: %s", actionToString(animation.action).c_str());

				
				const SpriteSheet& ss = animator.getSpritesheet(animation);

				StringBuffer32 spriteName = TextureManager::Get()->getTextureName(ss.texture);
				ImGui::Text("SpriteSheet: %s", spriteName.c_str());
				ImGui::VectorText("Frame Size", ss.frameSize);
				ImGui::VectorText("Object Size", ss.objectSize);
				ImGui::VectorText("Boundaries", ss.boundaries);

				std::vector<StringBuffer32> animations;
				for (u32 i = 0; i < animator.mAnimations.size(); i++)
				{
					ActionState action = animator.mAnimations[i].action;
					animations.push_back(actionToString(action).c_str());
				}

				ImGui::TreePop();
			}

			s_usingPlaylist = false;
			if (ImGui::TreeNode("Playlist Mode"))
			{
				// Playlist
				s_usingPlaylist = true;
				if (s_usingPlaylist)
				{
					Animator& animator = anim.animator;
					Animation* activeAnim = SetAnimation(animator);
					bool has_animation = activeAnim != nullptr;
					if (!has_animation)
						ImGui::BeginDisabled();

					float frame_time = activeAnim ? activeAnim->frameTime : 0.0f;
					ImGui::SliderFloat("Frame Time", &frame_time, 0.0f, 1.0f);
					if (activeAnim)
						activeAnim->frameTime = frame_time;

					char button_label1[64];
					snprintf(button_label1, 64, s_playSingleAnimation ? "Stop %s" : "Play %s", "single animation");
					if (ImGui::Button(button_label1))
					{
						s_playSingleAnimation = !s_playSingleAnimation;
					}
					ImGui::SameLine();

					char button_label2[64];
					snprintf(button_label2, 64, s_playingPlaylist ? "Stop %s" : "Play %s", "Animation Playlist");
					if (ImGui::Button(button_label2))
					{
						s_playingPlaylist = !s_playingPlaylist;
					}

					bool running_animation = s_playingPlaylist || s_playSingleAnimation;
					if (running_animation)
					{
						if (s_animationPlaylist.size() == 0)
						{
							s_playingPlaylist = false;
							s_playSingleAnimation = false;
						}

						FrameRateController& frc = FrameRateController::Get();
						if (animator.RunActive(frc.delta(), s_forceLooping))
						{
							if (s_playingPlaylist)
							{
								s_playlistIndex = (s_playlistIndex + 1) % s_animationPlaylist.size();
								animator.mFrameIndex = 0;
							}

							s_playSingleAnimation = false;
						}
					}

					ImGui::Checkbox("Force Looping", &s_forceLooping);

					if (running_animation)
						ImGui::BeginDisabled();

					if (ImGui::Button("<- Previous Frame"))
					{
						int prev_frame = animator.mFrameIndex - 1;
						if (prev_frame < 0)
						{
							s_playlistIndex = s_playlistIndex - 1;
							if (s_playlistIndex < 0)
								s_playlistIndex = s_animationPlaylist.size() - 1;

							activeAnim = SetAnimation(animator);

							prev_frame = activeAnim->frameCount - 1;
						}

						animator.mFrameIndex = prev_frame;
					}

					ImGui::SameLine();
					if (ImGui::Button("Next Frame ->"))
					{
						int next_frame = animator.mFrameIndex + 1;
						if (next_frame >= activeAnim->frameCount)
						{
							s_playlistIndex = (s_playlistIndex + 1) % s_animationPlaylist.size();
							next_frame = 0;
						}

						animator.mFrameIndex = next_frame;
					}

					activeAnim = SetAnimation(animator);


					int frame = (int)animator.mFrameIndex;
					ImGui::SliderInt("Frame", &frame, 0, activeAnim ? activeAnim->frameCount - 1 : 0);
					animator.mFrameIndex = (u32)frame;

					if (running_animation)
						ImGui::EndDisabled();

					if (!has_animation)
						ImGui::EndDisabled();

					for (u32 i = 0; i < s_animationPlaylist.size(); i++)
					{
						ImGui::PushID(i);
						if (ImGui::Button("-"))
						{
							s_animationPlaylist.erase(s_animationPlaylist.begin() + i);
							ImGui::PopID();
							break;
						}
						ImGui::SameLine();

						ActionState action = s_animationPlaylist[i].action;

						// select this animation
						if (ImGui::Button(actionToString(action).c_str()))
						{
							s_playlistIndex = i;
							animator.mFrameIndex = 0;
						}

						if (s_playlistIndex == i)
						{
							ImGui::SameLine();
							ImGui::Text(" <-- active");
						}
						ImGui::PopID();
					}

					std::vector<ActionState> actions;
					for (u32 i = 0; i < animator.mAnimations.size(); i++)
					{
						ActionState action = animator.mAnimations[i].action;
					}

					if (ImGui::BeginCombo("Add animation to playlist", "", 0))
					{
						for (u32 i = 0; i < animator.mAnimations.size(); i++)
						{
							ActionState action = animator.mAnimations[i].action;
							//const bool is_selected = action == activeAnim->action;

							char symbol = ' ';
							char dir = ' ';

							VectorI direction = animator.mAnimations[i].direction;
							bool flipable = animator.mAnimations[i].canFlip;
							if(direction.x != 0)
							{
								dir = 'x';
								if(flipable)
								{
									symbol = '*';
								}
								else
								{
									symbol = direction.x > 0 ? '+' : '-';
								}
							}
							else if(direction.y != 0)
							{
								dir = 'y';
								if(flipable)
								{
									symbol = '*';
								}
								else
								{
									symbol = direction.y > 0 ? '+' : '-';
								}
							}

							StringBuffer64 buffer;
							sprintf(buffer.buffer(), "%c%c: %s", symbol, dir, actionToString(action).c_str());

							if (ImGui::Selectable(buffer.c_str(), false))
							{
								s_animationPlaylist.push_back(animator.mAnimations[i]);
							}
						}

						ImGui::EndCombo();
					}
				}

				ImGui::TreePop();
			}
		}
		ImGui::PopID();
	}

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
		const RectF render_rect = ECS::GetRenderRect(entity);
			
		if (ImGui::TreeNode("Component Data"))
		{
			StringBuffer32 spriteName = TextureManager::Get()->getTextureName(sprite.texture);
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
			ImGui::Checkbox("Render Rect", &s_renderRects.render);
			if(s_renderRects.render)
			{	
				DebugDraw::RectOutline(render_rect, Colour::Green);
			}
			
			ImGui::Checkbox("Object Rect", &s_renderRects.object);
			if(s_renderRects.object)
			{
				RectF object_rect = ECS::GetObjectRect(entity);
				DebugDraw::RectOutline(object_rect, Colour::Purple);

				VectorF flip_point = ECS::GetObjectRect(entity).Center();
				DebugDraw::Point(flip_point, 5.0f, Colour::White);
			}

			ImGui::Checkbox("Collider Rect", &s_renderRects.collider);
			if(s_renderRects.collider)		
			{
				RectF collider_rect = ECS::GetColliderRect(entity);
				DebugDraw::RectOutline(collider_rect, Colour::Blue);
			}

			ImGui::TreePop();
		}
	}

	ImGui::PopID();

	return type;
}