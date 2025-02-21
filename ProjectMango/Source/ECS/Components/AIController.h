#pragma once

namespace ECS
{
	struct AIController
	{
		COMPONENT_TYPE(AIController)

		Entity target;

		bool canMoveToTarget;

		TimerF cooldownTimer;
		float attackCooldownTime;

		VectorF VectorToTarget() const;
		bool CanMoveForward(int ease_factor, float dt) const;
	};
}