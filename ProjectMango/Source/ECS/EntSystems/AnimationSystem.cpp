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

			UpdateAnimator(animator, dt);
			animator.SetActiveSpriteFrame(sprite);

			if( Collider* collider = GetComponent(Collider, entity) )
			{
				const Animation& animation = animator.GetActiveAnimation();

				if(animation.entityColliderEndPos != c_invalidVector)
				{
					const VectorF relative_movement = animation.entityColliderEndPos - animation.entityColliderPos;
					VectorF movement = relative_movement * transform.size;
					if(sprite.IsFlipped())
						movement = movement * -1.0f;
					
					const float animation_time = (float)animation.frameCount * animation.frameTime;
					const float frames = animation_time / dt;
					const VectorF movement_dt = movement / frames;
									
					collider->forward = collider->forward + movement_dt;
					transform.renderOffset -= movement_dt;
				}
				//else if(animation.entityColliderPos.isPositive() && animation.entityColliderSize.isPositive())
				{
					//if (sprite.rotation != 0)
					//{
					//	VectorF actual_center = VectorF(0.5f, 0.5f);
					//	VectorF real_to_visual_center = animation.entityColliderPos;// -actual_center;

					//	real_to_visual_center.rotateVector(sprite.rotation, actual_center);

					//	// looks like its doing the right thing but the rotation about point doesnt seem correct
					//	// or something to do with the positioning doesn look right
					//	collider->SetRelativeRect(real_to_visual_center, animation.entityColliderSize);
					//}
					//else
					{
						collider->SetRelativeRect(animation.entityColliderPos, animation.entityColliderSize);
					}

					//if(!animation.entityColliderPos.hasNegative())

					//collider->SetRelativeRect(animation.entityColliderPos, animation.entityColliderSize);
					transform.renderOffset = VectorF::zero();
				}
			}
		}
	}
}