#pragma once

#include <unordered_map>
#include <vector>

#include "Debugging/Logging.h"
#include "EntityCommon.h"

namespace ECS
{
	constexpr u32 c_pageLimit = 10;

	struct ComponentArrayBase
	{
		virtual ~ComponentArrayBase() = default;
	};
		
	template<class T>	
	struct Page
	{
		Page() : components(nullptr), pageSize(0), size(0), pageIndex(0) { }
		
		~Page() 
		{ 
			delete[] components; 
		}

		Page(u32 page_size, u32 page_index) : pageSize(page_size), size(0),	pageIndex(page_index)
		{
			components = new T[page_size]();
		}

		void Init(u32 page_size, u32 page_index)
		{
			components = new T[page_size];
			pageSize = page_size;
			size = 0; 
			pageIndex = page_index;
		}

		bool Unused() const { return components == nullptr; }
		bool HasSpace() const { return size < pageSize; }

		T* components;

		// the size of the page, cant be adjusted
		u32 pageSize;

		// how much of the page is currently used up
		u32 size;

		// which page is this, the first = 0 etc.
		u32 pageIndex;
	};

	template<class T>
	struct ComponentArray : public ComponentArrayBase
	{
		ComponentArray(u32 reserve_size) : componentPageSize(reserve_size) { }

		~ComponentArray()
		{
			entityToComponent.clear();
			componentToEntity.clear();
		}
		
		template<class T>
		T& InsertComponent(u32 entity)
		{
			Page<T>* target_page = nullptr;

			// first try get an already active page and insert there
			for (u32 i = 0; i < c_pageLimit; i++)
			{
				if (componentPages[i].HasSpace())
				{
					target_page = &componentPages[i];
					break;
				}
			}

			// otherwise get a new page
			if (!target_page)
			{
				for (u32 i = 0; i < c_pageLimit; i++)
				{
					if (componentPages[i].Unused())
					{
						componentPages[i].Init(componentPageSize, i);
						target_page = &componentPages[i];
						break;
					}
				}
			}

			ASSERT(target_page, "Run out of page space, either increase page size or allow more than %d pages", c_pageLimit);

			u32 component_index = target_page->pageIndex * componentPageSize + target_page->size;

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
			ASSERT(entityToComponent.count(entity) > 0, "Entity %d does not have an entry in this component list", entity);

			u32 component_index = entityToComponent[entity];

			int page_index = component_index / componentPageSize;
			int page_entry_index = component_index % componentPageSize;

			return componentPages[page_index].components[page_entry_index];
		}

		T& GetComponentByIndex(u32 component_index) const
		{
			int page_index = component_index / componentPageSize;
			int page_entry_index = component_index % componentPageSize;

			return componentPages[page_index].components[page_entry_index];
		}
		
		template<class T>
		void RemoveComponent(Entity entity)
		{
			//Page& page = GetPage(entity);
			u32 component_index = entityToComponent[entity];
			u32 page_index = component_index / componentPageSize;
			Page<T>& page = componentPages[page_index];

			// the back most component can replace the one we want to remove
			// replace the item we're removing with the last item in the page
			u32 page_entry_index = component_index % componentPageSize;
			page.components[page_entry_index] = page.components[page.size - 1];

			// mark it as dead just in case
			page.components[page.size - 1].entity = EntityInvalid;

			// update the entity and component indexes
			u32 last_component_index = page_index * componentPageSize + page.size - 1;
			Entity moving_entity = componentToEntity[last_component_index];
			componentToEntity[component_index] = moving_entity;
			entityToComponent[moving_entity] = component_index;

			// remove the now unused map entries
			entityToComponent.erase(entity);
			componentToEntity.erase(last_component_index);
			page.size--;
		}
				
		u32 GetComponentIndex(Entity entity) const
		{
			ASSERT(entityToComponent.count(entity) > 0, "Entity does not have an entry in this component list");
			return entityToComponent.at(entity);
		}

		u32 Count() const
		{
			return (u32)entityToComponent.size();
		}

		// mapping from an entity id to a components array index;
		std::unordered_map<Entity, u32> entityToComponent;
		std::unordered_map<u32, Entity> componentToEntity;

		Page<T> componentPages[c_pageLimit];

		u32 componentPageSize;
	};
}