#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class StateResolutionSystem : public EntitySystem
	{
	public:
		StateResolutionSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}