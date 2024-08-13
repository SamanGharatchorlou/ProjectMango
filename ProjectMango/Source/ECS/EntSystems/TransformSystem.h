#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class TransformSystem : public EntitySystem
	{
	public:
		TransformSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}