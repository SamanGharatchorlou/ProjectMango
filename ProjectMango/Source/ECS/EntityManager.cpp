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
		
		if(EntityData* entity_data = GetComponent(EntityData, entity))
		{
			for( int i = 0; i < entity_data->children.size(); i++ )
			{
				KillEntity(entity_data->children[i--]);
			}

			if(EntityData* parent_entity_data = GetComponent(EntityData, entity_data->parent))
			{
				std::vector<Entity>& vec = parent_entity_data->children;
				vec.erase(std::remove(vec.begin(), vec.end(), entity), vec.end());
			}
		}

		RemoveAllComponents(entity);
		archetypes[entity] = ArchetypeInvalid;
	}
}