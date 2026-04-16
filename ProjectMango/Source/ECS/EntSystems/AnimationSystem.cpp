#include "pch.h"
#include "AnimationSystem.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"

namespace ECS
{
	void AnimationSystem::UpdateAnimator(ECS::Animator& animator, float dt)
	{
		// update animation
		if(animator.state == TimeState::Running)
			animator.timer += dt;

		const Animation& active_animation = animator.animations->at(animator.activeAnimation);
		const float frame_time = active_animation.frameTime * ( 1 + animator.randomisedFrameTimeVariation);

		int next_frame = animator.frameIndex;
		if(animator.timer > active_animation.frameTime)
		{
			next_frame++;

			if(next_frame >= active_animation.frameCount)
			{
				animator.loopCount++;

				if(active_animation.looping)
					next_frame = 0;
				else
					animator.state = TimeState::Paused; // Stopped?
			}

			animator.frameIndex = (u32)Maths::clamp(next_frame, 0, active_animation.frameCount - 1);;
			animator.timer = 0;
		}
	}

	void StartAnimation(Animator& animator, Action::Enum action)
	{
		for( u32 i = 0; i < animator.animations->size(); i++ )
		{
			if(animator.animations->at(i).action == action)
			{
				animator.state = TimeState::Running;

				animator.activeAnimation = i;
				animator.frameIndex = 0;
				animator.loopCount = 0;
				animator.timer = 0;

				return;
			}
		}

		animator.activeAnimation = -1;
		DebugPrintOnce(Warning, "No animation found for action %s", ActionToString(action));
	}

	void AnimationSystem::Update(float dt)
	{
		for (Entity entity : entities)
		{
			// debug break point
			if(IsSelectedDebugEntity(entity))
				int a = 4;
			
			Action::Enum active_action = Action::None;

			Animator& animator = GetComponentRef(Animator, entity);
			if(animator.IsValid())
			{
				const Animation& active_animation = animator.animations->at(animator.activeAnimation);
				active_action = active_animation.action;
			}

			EntityState* entity_state = GetComponent(EntityState, entity);
			if( entity_state && (entity_state->current != active_action) )
			{
				Action::Enum next_animation = entity_state->current;
				if(!animator.HasAnimation(next_animation))
				{
					next_animation = Action::Inactive;
				}

				StartAnimation(animator, next_animation);
			}
			else
				UpdateAnimator(animator, dt);
		}
	}
}