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
			EntityState& state = GetComponentRef(EntityState, entity);
			BehaviourMap& behaviours = GetComponentRef(BehaviourMap, entity);
			BehaviourState& b_state = GetComponentRef(BehaviourState, entity);

			if(behaviours.updates.size() == 0)
			{
				DebugPrintOnce(Warning, "behaviour map for entity is empty");
			}

			// exit
			if(state.justChanged)
			{
				b_state.Reset();

				behaviours.attemptEnterFunction = true;

				Action::Enum previous_state = state.backlog.back();
				if(behaviours.exits.contains(previous_state))
				{
					behaviours.exits.at(previous_state)(entity);
					continue;
				}
			}


			// enter
			if(behaviours.attemptEnterFunction)
			{
				behaviours.attemptEnterFunction = false;

				if(behaviours.enters.contains(state.current))
				{
					behaviours.enters.at(state.current)(entity);
					continue;
				}
			}

			// update
			if(behaviours.updates.contains(state.current))
			{
				behaviours.updates.at(state.current)(entity);
				continue;
			}
			
			// attempt to execute a default behaviour
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
