#pragma once

#include "ECS/SystemManager.h"

namespace ECS
{
	class UISystem : public EntitySystem
	{
	public:
		UISystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;

		//static ECS::Entity cursor;
	};
}