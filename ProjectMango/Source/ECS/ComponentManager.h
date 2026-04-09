#pragma once

#include "ComponentArray.h"

namespace ECS
{
	struct ComponentManager
	{
		ComponentManager()
		{
			// TODO: WARNING NEED TO UPDATE THIS U64
			memset(componentArrays, 0, sizeof(ComponentArrayBase*) * MaxComponentIdCount);
		}

		void Close()
		{
			for( u32 i = 0; i < MaxComponentIdCount; i++ )
			{
				if(componentArrays[i])
				{
					delete componentArrays[i];
					componentArrays[i] = nullptr;
				}
			}
		}

		template<class T>
		void Register(u32 reserve_size)
		{
			ComponentID component_id = GetComponentID<T>();
			ASSERT(componentArrays[component_id] == nullptr, "Component (%d) hasnt been registered but has a component array already", component_id);
			componentArrays[component_id] = new ComponentArray<T>(reserve_size);
		}

		template<class T>
		T& AddComponent(Entity entity)
		{
			ComponentID component_id = GetComponentID<T>();
			return ((ComponentArray<T>*)componentArrays[component_id])->InsertComponent<T>(entity);
		}

		template<class T>
		void RemoveComponent(Entity entity)
		{
			ComponentID component_id = GetComponentID<T>();
			((ComponentArray<T>*)componentArrays[component_id])->RemoveComponent<T>(entity);
		}

		template<class T>
		T& GetComponent(Entity entity)
		{
			ComponentID component_id = GetComponentID<T>();
			return ((ComponentArray<T>*)componentArrays[component_id])->GetComponent(entity);
		}

		ComponentArrayBase* componentArrays[MaxComponentIdCount];
	};
}