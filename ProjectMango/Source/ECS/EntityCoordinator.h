#pragma once

#include "ComponentManager.h"
#include "SystemManager.h"
#include "EntityManager.h"


//#if ENTITY_LOGGING
//#define CreateEntity(name) CreateNewEntity(name)
//#else
//#define CreateEntity(name) CreateNewEntity()
//#endif

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

		template<class T>
		void RegisterComponent(Component::Type type, u32 reserve_size) { components.Register<T>(type, reserve_size); }

		template<class T>
		void RegisterAndSystem(Archetype type) { systems.RegisterAnd<T>(type); }

		template<class T>
		void RegisterOrSystem(Archetype type) { systems.RegisterOr<T>(type); }

		Entity CreateNewEntity() { return entities.CreateEntityId(); }

		bool IsAlive(Entity entity) const { return entity != EntityInvalid && entities.GetAchetype(entity) != ArchetypeInvalid; }

		bool HasComponent(Entity entity, Component::Type type) 
		{ 
			return entity != EntityInvalid && entities.HasComponent(entity, type);
		}

		template<class T>
		T& AddComponent(Entity entity, Component::Type type)
		{
			if(HasComponent(entity, type))
				return components.GetComponent<T>(entity, type);

			ASSERT(entity != EntityInvalid, "invaid entity, make sure to create a new one first");
			T& comp = components.AddComponent<T>(entity, type);
			entities.AddComponent(entity, type);

			Archetype archetype = entities.GetAchetype(entity);
			systems.EntityAddType(entity, archetype);

			return comp;
		}
		 
		template<class T>
		void RemoveComponent(Entity entity, Component::Type type)
		{
			if (entity == EntityInvalid || !HasComponent(entity, type))
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

		void UpdateSystems(float dt);

		EntityManager entities;
		ComponentManager components;
		SystemManager systems;
	};

#define RegisterComponent(compType, reserve) ecs->RegisterComponent<ECS::compType>(ECS::compType::type(), reserve)

#define AddComponent(compType, entity) ecs->AddComponent<ECS::compType>(entity, ECS::compType::type())
#define RemoveComponent(compType, entity) ecs->RemoveComponent<ECS::compType>(entity, ECS::compType::type())

#define HasComponent(compType, entity) ecs->HasComponent(entity, ECS::compType::type())
#define GetComponent(compType, entity) ecs->GetComponent<ECS::compType>(entity, ECS::compType::type())
#define GetComponentRef(compType, entity) ecs->GetComponentRef<ECS::compType>(entity, ECS::compType::type())

#define GetAllComponents(compType) ecs->GetComponents<ECS::compType>(ECS::compType::type())
}
