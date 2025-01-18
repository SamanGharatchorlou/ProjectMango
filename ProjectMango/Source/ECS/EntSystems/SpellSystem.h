#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class SpellSystem : public EntitySystem
	{
	public:
		SpellSystem(Archetype type) : EntitySystem(type) {}

		void Update(float dt) override;
	};
}