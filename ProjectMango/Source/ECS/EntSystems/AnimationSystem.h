#pragma once
#include "ECS/SystemManager.h"

namespace ECS
{
	struct Animator;

	class AnimationSystem : public EntitySystem
	{
	public:
		AnimationSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;

		static void UpdateAnimator(ECS::Animator& animator, float dt);
	};
}