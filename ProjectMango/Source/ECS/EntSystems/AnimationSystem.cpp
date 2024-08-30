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

			if( Collider* collider = ecs->GetComponent(Collider, entity) )
			{
				const Animation& animation = animator.GetActiveAnimation();
				if(animation.entityColliderPos.isPositive() && animation.entityColliderSize.isPositive())
					collider->SetRelativeRect(animation.entityColliderPos, animation.entityColliderSize);
			}
		}
	}
}