#pragma once

namespace ECS
{
	struct AIController
	{
		COMPONENT_TYPE(AIController)

		Entity target = EntityInvalid;

		bool moveToTarget = true;

		bool isAlert = false;

		TimerF cooldownTimer;
		float attackCooldownTime = 1.0f;

		float DistanceToTargetSquared() const;
		bool CanMoveForward(int ease_factor, float dt) const;
	};
}