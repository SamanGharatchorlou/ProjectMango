#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class ComponentUpdateSystem : public EntitySystem
	{
	public:
		ComponentUpdateSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}