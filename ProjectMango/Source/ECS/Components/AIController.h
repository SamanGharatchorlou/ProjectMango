#pragma once

namespace ECS
{
	struct AIController
	{
		COMPONENT_TYPE(AIController)

		Entity target;

		bool moveToTarget;
		bool isAlert;

		TimerF cooldownTimer;
		float attackCooldownTime;

		float DistanceToTargetSquared() const;
		bool CanMoveForward(int ease_factor, float dt) const;
	};
}