#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class TileMapSystem : public EntitySystem
	{
	public:
		TileMapSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}
