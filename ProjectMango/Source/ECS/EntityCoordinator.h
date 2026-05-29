#pragma once

#include "ComponentManager.h"
#include "SystemManager.h"
#include "EntityManager.h"

namespace ECS
{
	struct EntityCoordinator
	{
		EntityCoordinator() { }

		~EntityCoordinator()
		{
			Close();
		}


		void Close()
		{
			entities.Close();
			components.Close();
			systems.Close();
		}

		void DestroyAllEntities()
		{
			entities.Close();
			components.RemoveAllEntities();
			systems.RemoveAllEntities();
		}

		template<class T>
		void RegisterComponent(u32 reserve_size) { components.Register<T>(reserve_size); }

		template<class T>
		void RegisterAndSystem(Archetype type) { systems.RegisterAnd<T>(type); }

		template<class T>
		void RegisterOrSystem(Archetype type) { systems.RegisterOr<T>(type); }

		Entity CreateNewEntity() { return entities.CreateEntityId(); }

		bool IsAlive(Entity entity) const { return entity != EntityInvalid && entities.GetAchetype(entity) != ArchetypeInvalid; }

		template<class T>
		bool HasComponent(Entity entity)
		{ 
			return entity != EntityInvalid && entities.HasComponent(entity, GetComponentID<T>());
		}

		template<class T>
		T& AddComponent(Entity entity)
		{
			if(HasComponent<T>(entity))
				return components.GetComponent<T>(entity);

			ASSERT(entity != EntityInvalid, "invaid entity, make sure to create a new one first");
			T& comp = components.AddComponent<T>(entity);
			entities.AddComponent(entity, GetComponentID<T>());

			Archetype archetype = entities.GetAchetype(entity);
			systems.EntityAddType(entity, archetype);

			return comp;
		}
		 
		template<class T>
		void RemoveComponent(Entity entity)
		{
			if (entity == EntityInvalid || !HasComponent<T>(entity))
				return;

			components.RemoveComponent<T>(entity);

			ComponentID component_id = GetComponentID<T>();
			entities.RemoveComponent(entity, component_id);

			Archetype archetype = entities.GetAchetype(entity);
			systems.EntityRemoveType(entity, component_id, archetype);
		}

		template<class T>
		T& GetComponentRef(Entity entity)
		{
			ASSERT(entity != EntityInvalid, "invaid entity, make sure to create a new one first");
			return components.GetComponent<T>(entity);
		}

		template<class T>
		T* GetComponent(Entity entity)
		{
			ComponentID component_id = GetComponentID<T>();
			if(entity != EntityInvalid && entities.HasComponent(entity, component_id))
				return &components.GetComponent<T>(entity);
			
			return nullptr;
		}

		template<class T>
		T& GetOrAddComponent(Entity entity)
		{
			ComponentID component_id = GetComponentID<T>();
			if(entity != EntityInvalid && entities.HasComponent(entity, component_id))
				return components.GetComponent<T>(entity);

			return AddComponent<T>(entity);
		}

		template<class T>
		ComponentArray<T>& GetComponents() { return *static_cast<ComponentArray<T>*>(components.componentArrays[GetComponentID<T>()]); }
		
		void InitSystems();
		void UpdateSystems(float dt);

		void SetSystemPaused(Archetype type, bool is_paused);
		void ToggleSystemPaused(Archetype type);

		EntityManager entities;
		ComponentManager components;
		SystemManager systems;
	};

#define RegisterComponent(compType, reserve) ecs->RegisterComponent<ECS::compType>(reserve)

#define AddComponent(compType, entity) ecs->AddComponent<ECS::compType>(entity)
#define RemoveComponent(compType, entity) ecs->RemoveComponent<ECS::compType>(entity)

#define HasComponent(compType, entity) ecs->HasComponent<ECS::compType>(entity)
#define GetComponent(compType, entity) ecs->GetComponent<ECS::compType>(entity)
#define GetComponentRef(compType, entity) ecs->GetComponentRef<ECS::compType>(entity)
#define GetOrAddComponent(compType, entity) ecs->GetOrAddComponent<ECS::compType>(entity)

#define GetAllComponents(compType) ecs->GetComponents<ECS::compType>()
}
