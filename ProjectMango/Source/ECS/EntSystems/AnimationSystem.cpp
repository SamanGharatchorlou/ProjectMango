#include "pch.h"
#include "AnimationSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"

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
					animator.frameIndex = 0;
				else
					animator.state = TimeState::Paused; // Stopped?
			}

			animator.frameIndex = Maths::clamp(animator.frameIndex, (u32)0, (u32)active_animation.frameCount - 1);
			animator.timer = 0;
		}
	}

	void StartAnimation(Animator& animator, Action::Enum action)
	{
		for( u32 i = 0; i < animator.animations.size(); i++ )
		{
			if(animator.animations[i].action == action)
			{
				animator.state = TimeState::Running;

				animator.activeAnimation = i;
				animator.frameIndex = 0;
				animator.loopCount = 0;
				animator.timer = 0;

				return;
			}
		}

		DebugPrint(Warning, "No animation found for action %s", ActionToString(action));
	}

	void AnimationSystem::Update(float dt)
	{
		for (Entity entity : entities)
		{
			// debug break point
			if(DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			Transform& transform = GetComponentRef(Transform, entity);
			Animator& animator = GetComponentRef(Animator, entity);
			Sprite& sprite = GetComponentRef(Sprite, entity);

			if(!animator.IsValid())
				continue;
			
			Animation& active_animation = animator.animations[animator.activeAnimation];

			EntityState& character_state = GetComponentRef(EntityState, entity);
			if(character_state.current != active_animation.action)
			{
				StartAnimation(animator, character_state.current);
			}

			UpdateAnimator(animator, dt);
			animator.SetActiveSpriteFrame(sprite);

			if( Collider* collider = GetComponent(Collider, entity) )
			{
				const Animation& animation = animator.GetActiveAnimation();

				collider->SetRelativeRect(animation.entityColliderPos, animation.entityColliderSize);
				//transform.renderOffset = VectorF::zero();
			}
		}
	}
}