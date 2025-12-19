#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class AIControllerSystem : public EntitySystem
	{
	public:
		AIControllerSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}