#pragma once

#include "Debugging/Logging.h"
#include "EntityCommon.h"

namespace ECS
{
	struct EntitySystem
	{
		EntitySystem(Signature sig) : signature(sig) { }

		virtual void Update(float dt) = 0;

		std::vector<Entity> entities;
		Signature signature;
	};

	struct SystemManager
	{
		template<class T>
		void Register(Signature type)
		{
			for (u32 i = 0; i < entSystems.size(); i++)
			{
				if (type == entSystems[i]->signature)
				{
					const char* id = typeid(T).name();
					DebugPrint(Warning, "System %s alread registered with signature %d", id, type);
					return;
				}
			}

			entSystems.emplace_back(new T(type));
		}

		void EntityAddType(Entity entity, Signature type)
		{
			for (u32 i = 0; i < entSystems.size(); i++)
			{
				if (LockAndKey(entSystems[i]->signature, type))
				{
					bool already_exists = false;
					for (u32 ent = 0; ent < entSystems[i]->entities.size(); ent++)
					{
						// this entity is already in this system
						if (entSystems[i]->entities[ent] == entity) 
						{
							already_exists = true;
							break;                    
						}
					}

					if(!already_exists)
						entSystems[i]->entities.push_back(entity);
				}
			}
		}

		void EntityRemoveType(Entity entity, Component::Type type)
		{
			for (u32 i = 0; i < entSystems.size(); i++)
			{
				//if (LockAndKey(entSystems[i]->signature, type)) // this is wrong!
				if ( (entSystems[i]->signature & (u64)1 << type ))
				{
					const u32 ent_count = (u32)entSystems[i]->entities.size();
					for (int ent = 0; ent < ent_count; ent++)
					{
						if (entSystems[i]->entities[ent] == entity)
						{
							// copy the back element into the to be removed entities place, then pop the back
							entSystems[i]->entities[ent] = entSystems[i]->entities.back();
							entSystems[i]->entities.pop_back();
							break;
						}
					}
				}
			}
		}

		std::vector<EntitySystem*> entSystems;
	};
}