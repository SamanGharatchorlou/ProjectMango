#pragma once

#include "ComponentManager.h"
#include "SystemManager.h"
#include "EntityManager.h"


#if ENTITY_LOGGING
#define CreateEntity(name) CreateNewEntity(name)
#else
#define CreateEntity(name) CreateNewEntity()
#endif

namespace ECS
{
	struct EntityCoordinator
	{
		template<class T>
		void RegisterComponent(Component::Type type, u32 reserve_size) { components.Register<T>(type, reserve_size); }

		template<class T>
		void RegisterSystem(Archetype type) { systems.Register<T>(type); }

		Entity CreateNewEntity() { return entities.CreateEntityId(); }
#if ENTITY_LOGGING
		Entity CreateNewEntity(const char* name) { return entities.CreateEntityWithName(name); }
		Entity FindEntity(const char* name) { return entities.FindEntity(name); }
		const char* GetEntityName(Entity entity) { return entities.GetEntityName(entity); }
#endif

		bool IsAlive(Entity entity) const { return entity != EntityInvalid && entities.GetAchetype(entity) != ArchetypeInvalid; }

		template<class T>
		T& AddComponent(Entity entity, Component::Type type)
		{
			T& comp = components.AddComponent<T>(entity, type);
			comp.entity = entity;

			entities.AddComponent(entity, type);

			Archetype archetype = entities.GetAchetype(entity);
			systems.EntityAddType(entity, archetype);

			return comp;
		}

		template<class T>
		void RemoveComponent(Entity entity, Component::Type type)
		{
			if(const T* comp_ptr = GetComponent<T>(entity, type))
			{
				//Archetype archetype = entities.GetAchetype(entity);

				components.RemoveComponent<T>(entity, type);
				entities.RemoveComponent(entity, type);

				systems.EntityRemoveType(entity, type);
			}
		}

		template<class T>
		T& GetComponentRef(Entity entity, Component::Type type) { return components.GetComponent<T>(entity, type); }

		template<class T>
		T* GetComponent(Entity entity, Component::Type type) 
		{ 
			if(entities.HasComponent(entity, type))
				return &components.GetComponent<T>(entity, type); 
			
			return nullptr;
		}

		template<class T>
		ComponentArray<T>& GetComponents(Component::Type type) { return *static_cast<ComponentArray<T>*>(components.componentArrays[type]); }

		bool HasComponent(Entity entity, Component::Type type) 
		{ 
			return entities.HasComponent(entity, type); 
		}

		void UpdateSystems(float dt);

		EntityManager entities;
		ComponentManager components;
		SystemManager systems;
	};

#define RegisterComponent(compType, reserve) RegisterComponent<ECS::compType>(ECS::compType::type(), reserve)
#define AddComponent(compType, entity) AddComponent<ECS::compType>(entity, ECS::compType::type())
#define GetComponent(compType, entity) GetComponent<ECS::compType>(entity, ECS::compType::type())
#define GetComponentRef(compType, entity) GetComponentRef<ECS::compType>(entity, ECS::compType::type())
#define RemoveComponent(compType, entity) RemoveComponent<ECS::compType>(entity, ECS::compType::type())
#define HasComponent(compType, entity) HasComponent(entity, ECS::compType::type())
}
