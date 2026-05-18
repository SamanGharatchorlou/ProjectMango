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
		virtual void Clear() = 0;
		virtual u32 Count() const = 0;
		virtual const char* TypeName() const = 0;

	};
		
	template<class T>	
	struct Page
	{
		Page() : components(nullptr), pageSize(0), size(0), pageIndex(0) { }
		
		~Page() 
		{
			Close();
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
		void Close()
		{
			delete[] components;
			components = nullptr;
			size = 0;
			pageSize = 0;
		}

		bool Unused() const { return components == nullptr; }
		bool HasSpace() const { return size < pageSize; }

		T* components;

		// the size of the page, cant be adjusted once set
		u32 pageSize;

		// how much of the page is currently used up
		u32 size;

		// todo: remove this - dont need it
		// which page is this, the first = 0 etc.
		u32 pageIndex;

		Page& operator=(const Page&) = delete;
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

		void Clear() override
		{
			entityToComponent.clear();
			componentToEntity.clear();
			for (u32 i = 0; i < c_pageLimit; i++)
			{
				componentPages[i].Close();
			}
		}
		
		const char* TypeName() const override
		{
			//return "";
			return T::TypeName().c_str();
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

			T* component = target_page->components + target_page->size;

			// make sure we have 'clean' data
			*component = T{ };
			component->entity = entity;

			// setup the mappings for the component and its entity
			u32 component_index = target_page->pageIndex * componentPageSize + target_page->size;
			entityToComponent[entity] = component_index;
			componentToEntity[component_index] = entity;
						
			// increment size
			target_page->size++;

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
			u32 component_index = entityToComponent[entity];
			u32 page_index = component_index / componentPageSize;
			Page<T>& page = componentPages[page_index];

			// the back most component can replace the one we want to remove
			// replace the item we're removing with the last item in the page
			u32 page_entry_index = component_index % componentPageSize;
			
			// move last component into the freed slot
			page.components[page_entry_index] = std::move( page.components[page.size - 1] );

			// mark it as dead just in case
			page.components[page.size - 1].entity = EntityInvalid;

			// update the entity and component mappings
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

		u32 Count() const override
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