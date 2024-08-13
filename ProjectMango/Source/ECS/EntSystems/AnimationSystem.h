#pragma once
#include "ECS/SystemManager.h"

namespace ECS
{
	class AnimationSystem : public EntitySystem
	{
	public:
		AnimationSystem(Archetype type) : EntitySystem(type) { }

		void Update(float dt) override;
	};
}