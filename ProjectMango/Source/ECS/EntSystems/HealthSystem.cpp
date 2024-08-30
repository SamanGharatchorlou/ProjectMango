#include "pch.h"
#include "HealthSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/Components/ComponentDebugMenu.h"
#include "ECS/Components/PlayerController.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "Core/Helpers.h"

namespace ECS
{
	void HealthSystem::Update(float dt)
	{

		EntityCoordinator* ecs = GameData::Get().ecs;
		for (Entity entity : entities)
		{
			Health& health = ecs->GetComponentRef(Health, entity);
			for (u32 i = 0; i < health.ignoredDamaged.size(); i++)
			{
				// remove one dead entry at a time, we reorder and change the size of this list 
				// so this just keeps things simple and it doesnt matter if things hang around for a bit
				Entity ignored_entity = health.ignoredDamaged[i];
				if (!ecs->IsAlive(ignored_entity))
				{
					EraseSwap(health.ignoredDamaged, ignored_entity);
					break;
				}
			}
		}
	}
}