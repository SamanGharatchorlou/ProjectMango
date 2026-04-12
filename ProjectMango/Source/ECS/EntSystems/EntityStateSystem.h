#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class EntityStateSystem : public EntitySystem
	{
	public:
		EntityStateSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};

	bool CanChangeState(Entity entity);
	bool IsAttackState(Entity entity);
	bool FinishedAttacking(Entity entity);
}