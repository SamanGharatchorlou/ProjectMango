#pragma once
#include "pch.h"
#include "AnimationSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/Components/ComponentDebugMenu.h"
#include "ECS/Components/PlayerController.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/Animator.h"

namespace ECS
{
	void AnimationSystem::UpdateAnimator(ECS::Animator& animator, float dt)
	{
		// update animation
		if(animator.state == TimeState::Running)
			animator.timer += dt;

		Animation& active_animation = animator.animations[animator.activeAnimation];
		if(animator.timer > active_animation.frameTime)
		{
			animator.frameIndex++;

			if(animator.frameIndex >= active_animation.frameCount)
			{
				animator.loopCount++;

				if(active_animation.looping)
				{
					animator.frameIndex = 0;
				}
				else
				{
					animator.state = TimeState::Paused; // Stopped?
				}
			}

			animator.frameIndex = Maths::clamp(animator.frameIndex, (u32)0, (u32)active_animation.frameCount - 1);
			animator.timer = 0;
		}
	}

	void AnimationSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		for (Entity entity : entities)
		{
			// debug break point
			if(DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			Transform& transform = ecs->GetComponentRef(Transform, entity);
			Animator& animator = ecs->GetComponentRef(Animator, entity);
			Sprite& sprite = ecs->GetComponentRef(Sprite, entity);

			UpdateAnimator(animator, dt);
			animator.SetActiveSpriteFrame(sprite);

			//sprite.

			//bool using_playlist = DebugMenu::UsingPlaylist(entity);

			//Animator& animator = animation.animator;

			//if(!using_playlist)
			//	animator.RunActive(dt);
			//
			//Sprite& sprite = ecs->GetComponentRef(Sprite, entity);


			//VectorF pos = animator.getAnimationSubRect();
			//sprite.subRect = RectF(pos, animator.FrameSize());

			//if (using_playlist)
			//	continue;

			//// select the next action
			//ActionState action = state.action;
			//bool is_flipped = false;

			//// todo: perhaps rename this or put it into a namespace?
			//const ::Animation* anim = animator.getAnimation(action, state.facingDirection, is_flipped);
			//const ::Animation* active_anim = animator.activeAnimation();
			//
			//sprite.flip = is_flipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;

			//DebugMenu::SpriteFlipOverride(entity, sprite.flip);

			//if ( anim && anim != active_anim )
			//{
			//	animator.selectAnimation(*anim);

			//	const SpriteSheet& ss = animator.getSpritesheet(*anim);
			//	sprite.renderSize = ss.renderSize;
			//	sprite.texture = ss.texture;
			//}

			//
			//// update transform size, is this the wrong place to do it?
			//transform.rect.SetSize(sprite.renderSize);
		}
	}
}