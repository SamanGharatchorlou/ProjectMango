#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class CallbackSystem : public EntitySystem
	{
	public:
		CallbackSystem(Archetype type) : EntitySystem(type) { }
		
		void Init() override;
		void Update(float dt) override;

	};
}