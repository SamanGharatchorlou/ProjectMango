#pragma once

#include "ECS/SystemManager.h"


enum class ActionState;

namespace ECS
{
	struct AIController;

	class AIControllerSystem : public EntitySystem
	{
	public:
		AIControllerSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}