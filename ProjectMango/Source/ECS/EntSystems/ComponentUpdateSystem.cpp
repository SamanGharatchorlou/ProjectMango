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
			Entity the_entity = entity;
			if(Spawner* spawner = ecs->GetComponent(Spawner, the_entity))
			{
				spawner->Update();
			}
			if(Door* door = ecs->GetComponent(Door, the_entity))
			{
				door->Update();
			}
		}
	}
}
	