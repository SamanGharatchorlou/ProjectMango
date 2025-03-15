#pragma once

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
			// there can be an issue here sometimes, no idea why... but again maybe because
			// of the entity vector im using in the system update it might be reallocated mid way 
			// during the loop meaning the entity references i get are fucked, this here prevents the issue
			// and it seems to fix itself, so as i type this im starting to think im correct?
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
	};
}

