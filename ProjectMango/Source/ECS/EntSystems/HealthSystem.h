#pragma once
#include "ECS/SystemManager.h"

namespace ECS
{
	class HealthSystem : public EntitySystem
	{
	public:
		HealthSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}