#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class EntityStateSystem : public EntitySystem
	{
	public:
		EntityStateSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}