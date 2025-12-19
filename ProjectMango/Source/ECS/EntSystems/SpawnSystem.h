#pragma once

#include "ECS/SystemManager.h"


namespace ECS
{
	class SpawnSystem : public EntitySystem
	{
	public:
		SpawnSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;

		//std::vector<
	};
}
