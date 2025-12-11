#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class TurnActionSystem : public EntitySystem
	{
	public:
		TurnActionSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}