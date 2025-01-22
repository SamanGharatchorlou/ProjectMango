#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class CollisionSystem : public EntitySystem
	{
	public:
		CollisionSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;

		static void FindValidPosition(ECS::Entity entity);

		// destroyed next frame, so anything on it can do its thing
		// add a frame count? make this into its own part?
		std::vector<Entity> entitiesToDestroy;
	};
}