#pragma once

namespace ECS
{
	struct AIController
	{
		COMPONENT_TYPE(AIController)

		Entity target = EntityInvalid;

		bool moveToTarget = true;

		float attackCooldownTime = 1.0f;
		TimerF cooldownTimer;

		float DistanceToTarget() const;
	};
}