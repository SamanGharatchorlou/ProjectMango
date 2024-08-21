#include "pch.h"
#include "EntityManager.h"

// todo move this
#include "ECS/Components/ComponentsSetup.h"

namespace ECS
{
	void EntityManager::KillEntity(Entity entity)
	{
		if (entity == EntityInvalid)
			return;

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