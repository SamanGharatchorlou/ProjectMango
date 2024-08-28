#pragma once

#include "Core/stack.h"
#include "Characters/States/EnemyStates.h" // can try to forward decalare state

namespace ECS
{

	struct AIController
	{
		COMPONENT_TYPE(AIController)

		Entity target = EntityInvalid;

		u32 gotHitFrame = -1;
	};
}