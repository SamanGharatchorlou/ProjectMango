#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class PhysicsSystem : public EntitySystem
	{
	public:
		PhysicsSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}