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
			Entity the_entity = entity;
			if(Spawner* spawner = GetComponent(Spawner, the_entity))
			{
				spawner->Update();
			}
			if(Door* door = GetComponent(Door, the_entity))
			{
				door->Update();
			}			
			// dont think this is running, need to just fix what runs in this function
			if (Pickup* pick_up = GetComponent(Pickup, the_entity))
			{
				pick_up->Update();
			}
			if (DeathScentence* ds = GetComponent(DeathScentence, the_entity))
			{
				ds->Update(dt);
			}
			if (Arm* arm = GetComponent(Arm, the_entity))
			{
				arm->Update();
			}
		}
	}
}
	