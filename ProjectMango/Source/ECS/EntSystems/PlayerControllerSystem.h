#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class PlayerControllerSystem : public EntitySystem
	{
	public:
		PlayerControllerSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
		
		bool spawningPlayer = false;
		TimerF deathTimer;
	};
}