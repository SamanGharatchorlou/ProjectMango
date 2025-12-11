#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class InputSystem : public EntitySystem
	{
	public:
		InputSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;

		//static ECS::Entity cursor;
	};
}