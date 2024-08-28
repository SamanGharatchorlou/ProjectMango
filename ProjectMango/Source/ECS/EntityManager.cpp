#include "pch.h"
#include "EntityManager.h"

// todo move this
#include "ECS/Components/ComponentsSetup.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"

namespace ECS
{
	void EntityManager::KillEntity(Entity entity)
	{
		if (entity == EntityInvalid)
			return;
		
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		if(EntityData* entity_data = ecs->GetComponent(EntityData, entity))
		{
			for( u32 i = 0; i < entity_data->children.size(); i++ )
			{
				KillEntity(entity_data->children[i]);
			}

			if(EntityData* parent_entity_data = ecs->GetComponent(EntityData, entity_data->parent))
			{
				EraseSwap(parent_entity_data->children, entity);
			}
		}

		RemoveAllComponents(entity);
		archetypes[entity] = ArchetypeInvalid;

#if ENTITY_LOGGING
		for( auto iter = entityNames.begin(); iter != entityNames.end(); iter++ )
		{
			if(iter->first == entity)
			{
				entityNames.erase(iter);
				break;
			}
		}
#endif
	}
}