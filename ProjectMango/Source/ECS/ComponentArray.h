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
	struct Page
	{
		Page() { }
		Page(u32 page_size, u32 page_index)
		{
			components = new T[page_size]();
			pageSize = page_size;
			size = 0;
			pageIndex = page_index;
		}

		bool HasSpace() const { return size < pageSize; }

		T* components = nullptr;
		u32 pageSize = 0;

		// how much of the page is currently used up
		u32 size = 0;

		// which page is this, the first = 0 etc.
		u32 pageIndex = 0;
	};

	template<class T>
	struct ComponentArray : public ComponentArrayBase
	{
		ComponentArray(u32 reserve_size) : pageSize(reserve_size) { }
		
		template<class T>
		T& InsertComponent(u32 entity)
		{
			Page<T>* target_page = nullptr;
			for( u32 i = 0; i < componentPages.size(); i++ )
			{
				if(componentPages[i].HasSpace())
				{
					target_page = &componentPages[i];
				}
			}

			if(!target_page)
			{
				componentPages.emplace_back( pageSize, (u32)componentPages.size() );
				target_page = &(componentPages.back());
			}


			u32 component_index = target_page->pageIndex * pageSize + target_page->size;

			T* component = target_page->components + target_page->size;
			*component = T();
			component->entity = entity;

			// increment size
			target_page->size++;

			entityToComponent[entity] = component_index;
			componentToEntity[component_index] = entity;

			return *component;
		}

		T& GetComponent(Entity entity)
		{
			//T* my_thing = new T;
			//return *my_thing;
			ASSERT(entityToComponent.count(entity) > 0, "Entity %d does not have an entry in this component list", entity);

			u32 component_index = entityToComponent[entity];

			int page_index = component_index / pageSize;
			int page_entry_index = component_index % pageSize;

			return componentPages[page_index].components[page_entry_index];
		}

		T& GetComponentByIndex(u32 component_index) const
		{
			//T* my_thing = new T;
			//return *my_thing;
			int page_index = component_index / pageSize;
			int page_entry_index = component_index % pageSize;

			return componentPages[page_index].components[page_entry_index];
		}
		
		template<class T>
		void RemoveComponent(Entity entity)
		{
			//Page& page = GetPage(entity);
			u32 component_index = entityToComponent[entity];
			int page_index = component_index / pageSize;
			Page<T>& page = componentPages[page_index];

			u32 page_entry_index = component_index % pageSize;
			
			// the back most component can replace the one we want to remove
			// replace the item we're removing with the last item in the page
			//u32 page_entry_index = GetPageEntryIndex(entity);

			u32 last_component_index = page_entry_index * pageSize + page.size - 1;
			page.components[page_entry_index] = page.components[page.size - 1];
			page.size--;

			// mark it as dead just in case
			page.components[page.size - 1].entity = EntityInvalid;

			// index of the component we're just moved to the new spot
			//u32 last_component_index = (u32)page.components.size();

			// update the entity and component indexes
			Entity moving_entity = componentToEntity[last_component_index];
			componentToEntity[component_index] = moving_entity;
			entityToComponent[moving_entity] = component_index;

			// remove the now unused map entries
			entityToComponent.erase(entity);
			componentToEntity.erase(last_component_index);
		}
				
		u32 GetComponentIndex(Entity entity) const
		{
			ASSERT(entityToComponent.count(entity) > 0, "Entity does not have an entry in this component list");
			return entityToComponent.at(entity);
		}

		u32 Count() const
		{
			return entityToComponent.size();
		}

		// mapping from an entity id to a components array index;
		std::unordered_map<Entity, u32> entityToComponent;
		std::unordered_map<u32, Entity> componentToEntity;

		std::vector<Page<T>> componentPages;

		u32 pageSize;
	};
}