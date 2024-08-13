#pragma once

#include "ComponentArray.h"

namespace ECS
{
	struct ComponentManager
	{
		ComponentManager()
		{
			memset(componentArrays, 0, sizeof(ComponentArrayBase*) * Component::Count);
		}

		template<class T>
		void Register(Component::Type type, u32 reserve_size)
		{
			ASSERT(componentArrays[type] == nullptr, "Component (%d) hasnt been registered but has a component array already", (u32)type);
			componentArrays[type] = new ComponentArray<T>(reserve_size);
		}

		template<class T>
		T& AddComponent(Entity entity, Component::Type type)
		{
			return ((ComponentArray<T>*)componentArrays[type])->InsertComponent(entity);
		}

		template<class T>
		void RemoveComponent(Entity entity, Component::Type type)
		{
			((ComponentArray<T>*)componentArrays[type])->RemoveComponent(entity);
		}

		template<class T>
		T& GetComponent(Entity entity, Component::Type type)
		{
			return ((ComponentArray<T>*)componentArrays[type])->GetComponent(entity);
		}

		ComponentArrayBase* componentArrays[Component::Type::Count];
	};
}