#include "pch.h"
#include "ComponentUpdateSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"

namespace ECS
{
	void ComponentUpdateSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		for (Entity entity : entities)
		{
			if(Spawner* spawner = ecs->GetComponent(Spawner, entity))
			{
				spawner->Update();
			}
			if(Door* door = ecs->GetComponent(Door, entity))
			{
				door->Update();
			}
		}
	}
}
	