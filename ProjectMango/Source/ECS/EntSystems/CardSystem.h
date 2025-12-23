#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class CardSystem : public EntitySystem
	{
	public:
		CardSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}