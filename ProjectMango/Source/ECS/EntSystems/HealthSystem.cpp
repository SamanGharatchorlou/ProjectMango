#include "pch.h"
#include "HealthSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"

namespace ECS
{
	void HealthSystem::Init() { }

	void HealthSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			if (DeathScentence* ds = GetComponent(DeathScentence, entity))
			{
				ds->Update(dt);
			}
		}
	}
}