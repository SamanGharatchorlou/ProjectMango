#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class SpellSystem : public EntitySystem
	{
	public:
		SpellSystem(Archetype type) : EntitySystem(type) {}

		void Update(float dt) override;

		// destroyed next frame, so anything on it can do its thing
		// add a frame count? make this into its own part?
		std::vector<Entity> entitiesToDestroy;
	};
}