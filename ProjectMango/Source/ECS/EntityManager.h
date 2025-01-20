#pragma once

#include "EntityCommon.h"

namespace ECS
{
	struct EntityManager
	{
		EntityManager() : entityIdIndex(0)
		{
			memset(archetypes, (int)ArchetypeInvalid, sizeof(Archetype) * MaxEntityCount);
		}

		void Close()
		{
			memset(archetypes, (int)-1, sizeof(Archetype) * MaxEntityCount);
			entityIdIndex = 0;
		}

		Entity CreateEntityId()
		{
			Entity entityId = entityIdIndex;
			archetypes[entityId] = 0;
			entityIdIndex++;
			return entityId;
		}

		void KillEntity(Entity entity);

		bool HasComponent(Entity entity, Component::Type component) const
		{
			if(entity > entityIdIndex)
				return false;

			return archetypes[entity] != ArchetypeInvalid && archetypes[entity] & ((u64)1 << component);
		}

		void AddComponent(Entity entity, Component::Type component)
		{
			archetypes[entity] |= ((u64)1 << component);
		}

		void RemoveComponent(Entity entity, Component::Type component)
		{
			archetypes[entity] &= ~((u64)1 << component);
		}

		Archetype GetAchetype(Entity entity) const
		{
			return archetypes[entity];
		}

		Archetype archetypes[MaxEntityCount];
		Entity entityIdIndex;

#if ENTITY_LOGGING
		std::unordered_map<Entity, BasicString> entityNames;

		Entity CreateEntityWithName(const char* name = nullptr)
		{
			const Entity entityId = CreateEntityId();

			if(name)
				entityNames[entityId] = BasicString(name);

			return entityId;
		}

		Entity FindEntity(const char* name) 
		{
			for(auto iter = entityNames.begin(); iter != entityNames.end(); iter++) 
			{
				if(StringCompare(iter->second.c_str(), name)) 
				{
					return iter->first;
				}     
			}
		}

		const char* GetEntityName(Entity entity) 
		{
			if(entityNames.count(entity) > 0)
				return entityNames.at(entity).c_str();

			return nullptr;
		}
#endif
	};
}

