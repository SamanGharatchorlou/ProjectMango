#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class RenderSystem : public EntitySystem
	{
	public:
		RenderSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}