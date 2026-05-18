#pragma once

#include "Debugging/Logging.h"
#include "EntityCommon.h"

namespace ECS
{
	struct EntitySystem
	{
		EntitySystem(Signature sig) : signature(sig) { }
		
		virtual void Init() { };
		virtual void Update(float dt) = 0;

		std::vector<Entity> entities;
		Signature signature = 0;

		BasicString id;

		int orderIndex = -1;

		// need to implement this below
		// the signature can be an AND or OR match, defaults to OR
		bool signature_OR = false;
		bool paused = false;
	};

	struct SystemManager
	{
		int orderCounter = 0;

		void Close()
		{
			// shut down all systems
			for (u32 i = 0; i < entAndSystems.size(); i++)
			{
				delete entAndSystems[i];
			}
			// shut down all systems
			for (u32 i = 0; i < entOrSystems.size(); i++)
			{
				delete entOrSystems[i];
			}

			entAndSystems.clear();
			entOrSystems.clear();

			orderCounter = 0;
		}

		void RemoveAllEntities()
		{
			// shut down all systems
			for (u32 i = 0; i < entAndSystems.size(); i++)
			{
				entAndSystems[i]->entities.clear();
			}
			// shut down all systems
			for (u32 i = 0; i < entOrSystems.size(); i++)
			{
				entOrSystems[i]->entities.clear();
			}
		}

		template<class T>
		void RegisterAnd(Signature type)
		{
			for (u32 i = 0; i < entAndSystems.size(); i++)
			{
				if (type == entAndSystems[i]->signature)
				{
					const char* id = typeid(T).name();
					DebugPrint(Warning, "System %s alread registered with signature %d", id, type);
					return;
				}
			}


			T* system = new T(type);
			system->orderIndex = orderCounter++;
			system->id = typeid(T).name();

			entAndSystems.push_back(system);

			//entAndSystems.emplace_back(new T(type));
			//entAndSystems.back()->orderIndex = orderCounter++;
			//entAndSystems.back()->id = typeid(T).name();
		}

		template<class T>
		void RegisterOr(Signature type)
		{
			for (u32 i = 0; i < entOrSystems.size(); i++)
			{
				if (type == entOrSystems[i]->signature)
				{
					const char* id = typeid(T).name();
					DebugPrint(Warning, "System %s alread registered with signature %d", id, type);
					return;
				}
			}

			T* system = new T(type);
			system->orderIndex = orderCounter++;
			system->id = typeid(T).name();

			entOrSystems.push_back(system);

			//entOrSystems.emplace_back(new T(type));
			//entOrSystems.back()->orderIndex = orderCounter++;
			//entAndSystems.back()->id = typeid(T).name();
		}

		template<class T>
		T* GetSystem()
		{
			const char* id = typeid(T).name();
			for (u32 i = 0; i < entAndSystems.size(); i++)
			{
				if( StringCompare(entAndSystems[i]->id.c_str(), id))
					return (T*)entAndSystems[i];
			}
			for (u32 i = 0; i < entOrSystems.size(); i++)
			{				
				if( StringCompare(entOrSystems[i]->id.c_str(), id))
					return (T*)entOrSystems[i];
			}

			return nullptr;
		}

		void EntityAddType(Entity entity, Signature type)
		{
			for (u32 i = 0; i < entAndSystems.size(); i++)
			{
				if (LockAndKey(entAndSystems[i]->signature, type))
				{
					bool already_exists = false;
					for (u32 ent = 0; ent < entAndSystems[i]->entities.size(); ent++)
					{
						// this entity is already in this system
						if (entAndSystems[i]->entities[ent] == entity) 
						{
							already_exists = true;
							break;                    
						}
					}

					if(!already_exists)
						entAndSystems[i]->entities.push_back(entity);
				}
			}

			for (u32 i = 0; i < entOrSystems.size(); i++)
			{
				// check if there's at least 1 matching bit
				if ((entOrSystems[i]->signature & type) != 0)
				{
					bool already_exists = false;
					for (u32 ent = 0; ent < entOrSystems[i]->entities.size(); ent++)
					{
						// this entity is already in this system
						if (entOrSystems[i]->entities[ent] == entity)
						{
							already_exists = true;
							break;
						}
					}

					if (!already_exists)
						entOrSystems[i]->entities.push_back(entity);
				}
			}
		}

		void EntityRemoveType(Entity entity, ComponentID component_id)
		{
			for (u32 i = 0; i < entAndSystems.size(); i++)
			{
				// need to check if this entity could be part of this system, it might not even be
				// possible but we cant know that so we need to check all of them
				if ( (entAndSystems[i]->signature & (u64)1 << component_id))
				{
					const u32 ent_count = (u32)entAndSystems[i]->entities.size();
					for (int ent = 0; ent < ent_count; ent++)
					{
						if (entAndSystems[i]->entities[ent] == entity)
						{
							// copy the back element into the to be removed entities place, then pop the back
							entAndSystems[i]->entities[ent] = entAndSystems[i]->entities.back();
							entAndSystems[i]->entities.pop_back();
							break;
						}
					}
				}
			}
			for (u32 i = 0; i < entOrSystems.size(); i++)
			{
				// need to check if this entity could be part of this system
				if ((entOrSystems[i]->signature & (u64)1 << component_id))
				{
					const u32 ent_count = (u32)entOrSystems[i]->entities.size();
					for (int ent = 0; ent < ent_count; ent++)
					{
						if (entOrSystems[i]->entities[ent] == entity)
						{
							// copy the back element into the to be removed entities place, then pop the back
							entOrSystems[i]->entities[ent] = entOrSystems[i]->entities.back();
							entOrSystems[i]->entities.pop_back();
							break;
						}
					}
				}
			}
		}

		std::vector<EntitySystem*> entAndSystems;
		std::vector<EntitySystem*> entOrSystems;
	};
}