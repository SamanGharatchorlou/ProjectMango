#include "pch.h"
#include "EntityStateSystem.h"

#include "ECS/Components/AIComponents.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Animator.h"

namespace ECS
{
	// commits the state data i.e. applies the current state, respecting any blocks
	void EntityStateSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			EntityState& state = GetComponentRef(EntityState, entity);

			// default
			if(state.current == Action::None && state.next == Action::None)
			{
				state.next = Action::Idle;
			}

			// death
			if(DeathScentence* death = GetComponent(DeathScentence, entity))
			{

				state.next = Action::Death;
			}
						
			state.mustFinishAnimation = (state.current == Action::BasicAttack);

			bool can_change_state = !state.mustFinishAnimation;
			if(state.mustFinishAnimation)
			{
				if(Animator* animator = GetComponent(Animator, entity))
				{
					can_change_state = animator->loopCount > 0;
				}
			}

			state.justChanged = false;

			if(can_change_state)
			{
				if(state.current != state.next)
				{
					state.backlog.push_back(state.current);
					state.current = state.next;

					state.justChanged = true;
				}
			}
		}
	}
}