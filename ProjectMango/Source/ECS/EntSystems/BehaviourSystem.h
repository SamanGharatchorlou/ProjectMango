#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class BehaviourSystem : public EntitySystem
	{
	public:
		BehaviourSystem(Archetype type) : EntitySystem(type) { }

		void Init() override;
		void Update(float dt) override;
	};
}