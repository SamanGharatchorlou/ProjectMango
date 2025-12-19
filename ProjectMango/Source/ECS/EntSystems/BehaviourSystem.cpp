#include "pch.h"
#include "BehaviourSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/AIComponents.h"
#include "ECS/EntityCoordinator.h"

namespace ECS
{
	void BehaviourSystem::Init()
	{

	}

	void BehaviourSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			//AIController& aic = GetComponentRef(AIController, entity);
			EntityState& state = GetComponentRef(EntityState, entity);
			BehaviourMap& behaviours = GetComponentRef(BehaviourMap, entity);

			if(behaviours.updates.size() == 0)
			{
				DebugPrintOnce(Warning, "behaviour map for entity is empty");
			}

			bool did_run = false;
			if(state.justChanged)
			{
				Action::Enum previous_state = state.backlog.back();
				if(behaviours.exits.contains(previous_state))
				{
					behaviours.exits.at(previous_state)(entity);
					did_run = true;
				}
			}

			if(!did_run)
			{
				if(behaviours.updates.contains(state.current))
				{
					behaviours.updates.at(state.current)(entity);
					did_run = true;
				}
			}
			
			// attempt to execute a default behaviour
			if(!did_run)
			{
				if(behaviours.updates.contains(Action::Idle))
				{
					behaviours.updates.at(Action::Idle)(entity);
				}
				else if(behaviours.updates.contains(Action::Inactive))
				{
					behaviours.updates.at(Action::Inactive)(entity);
				}
			}
		}
	}
}
