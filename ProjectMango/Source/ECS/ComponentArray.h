#pragma once

#include <unordered_map>
#include <vector>

#include "Debugging/Logging.h"
#include "EntityCommon.h"

namespace ECS
{
	struct ComponentArrayBase
	{
		virtual ~ComponentArrayBase() = default;
	};

	template<class T>
	struct ComponentArray : public ComponentArrayBase
	{
		ComponentArray(u32 reserve_size)
		{
			components.reserve(reserve_size);
		}

		T& InsertComponent(Entity entity)
		{
			u32 index = (u32)components.size();

			components.emplace_back(T());

			entityToComponent[entity] = index;
			componentToEntity[index] = entity;

			return components.back();
		}

		void RemoveComponent(Entity entity)
		{
			// the back most component can replace the one we want to remove
			u32 component_index = entityToComponent[entity];
			components[component_index] = components.back();
			components.pop_back();

			// index of the component we're just moved to the new spot
			u32 last_component_index = (u32)components.size();

			// update the entity and component indexes
			Entity moving_entity = componentToEntity[last_component_index];
			componentToEntity[component_index] = moving_entity;
			entityToComponent[moving_entity] = component_index;

			// remove the now unused map entries
			entityToComponent.erase(entity);
			componentToEntity.erase(last_component_index);
		}

		T& GetComponent(Entity entity)
		{
			ASSERT(entityToComponent.count(entity) > 0 && components.size() > entityToComponent[entity], "Entity %d does not have an entry in this component list", entity);

			u32 index = entityToComponent[entity];
			return components[index];
		}
				
		u32 GetComponentIndex(Entity entity) const
		{
			//ASSERT(entityToComponent.count(entity) > 0 && components.size() > entityToComponent[entity], "Entity does not have an entry in this component list");
			return entityToComponent.at(entity);
		}

		// mapping from an entity id to a components array index;
		std::unordered_map<Entity, u32> entityToComponent;
		std::unordered_map<u32, Entity> componentToEntity;

		std::vector<T> components;
	};
}