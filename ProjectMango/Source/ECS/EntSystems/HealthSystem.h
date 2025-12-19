#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class HealthSystem : public EntitySystem
	{
	public:
		HealthSystem(Archetype type) : EntitySystem(type) { }

		void Init() override;
		void Update(float dt) override;
	};
}