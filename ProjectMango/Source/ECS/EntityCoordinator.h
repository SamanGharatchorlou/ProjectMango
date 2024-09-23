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
		void AddComponent(Entity entity, Component::Type type)
		{
			ASSERT(entity != EntityInvalid, "invaid entity, make sure to create a new one first");
			T& comp = components.AddComponent<T>(entity, type);
			comp.entity = entity;

			entities.AddComponent(entity, type);

			Archetype archetype = entities.GetAchetype(entity);
			systems.EntityAddType(entity, archetype);
		}

		template<class T>
		void RemoveComponent(Entity entity, Component::Type type)
		{
			if (entity == EntityInvalid)
				return;

			if(const T* comp_ptr = GetComponent<T>(entity, type))
			{
				components.RemoveComponent<T>(entity, type);
				entities.RemoveComponent(entity, type);

				systems.EntityRemoveType(entity, type);
			}
		}

		template<class T>
		T& GetComponentRef(Entity entity, Component::Type type) 
		{
			ASSERT(entity != EntityInvalid, "invaid entity, make sure to create a new one first");
			return components.GetComponent<T>(entity, type); 
		}

		template<class T>
		T* GetComponent(Entity entity, Component::Type type) 
		{ 
			if(entity != EntityInvalid && entities.HasComponent(entity, type))
				return &components.GetComponent<T>(entity, type); 
			
			return nullptr;
		}

		template<class T>
		ComponentArray<T>& GetComponents(Component::Type type) { return *static_cast<ComponentArray<T>*>(components.componentArrays[type]); }

		template<class T>
		void GetEntitiesWithComponent(Component::Type type, std::vector<Entity>& entities)
		{
			ComponentArray<T>& array = GetComponents<T>(type);
			for( u32 i = 0; i < array.components.size(); i++ )
			{
				entities.push_back(array.components[i].entity);
			}
		}

		bool HasComponent(Entity entity, Component::Type type) 
		{ 
			return entity != EntityInvalid && entities.HasComponent(entity, type);
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

#define GetEntitiesWithComponent(compType, entities) GetEntitiesWithComponent<ECS::compType>(ECS::compType::type(), entities)

}
