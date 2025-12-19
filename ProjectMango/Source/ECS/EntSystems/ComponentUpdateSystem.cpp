#include "pch.h"
#include "ComponentUpdateSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"

namespace ECS
{
	void ComponentUpdateSystem::Update(float dt)
	{
		for (Entity entity : entities)
		{
			//if(Spawner* spawner = GetComponent(Spawner, entity))
			//{
			//	spawner->Update();
			//}
			if(Door* door = GetComponent(Door, entity))
			{
				door->Update();
			}			
			if (DeathScentence* ds = GetComponent(DeathScentence, entity))
			{
				ds->Update(dt);
			}
		}
	}
}
	