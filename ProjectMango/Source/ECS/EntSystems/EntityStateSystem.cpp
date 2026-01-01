#include "pch.h"
#include "EntityStateSystem.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GraphicComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"

namespace ECS
{
	// commits the state data i.e. applies the current state, respecting any blocks
	void EntityStateSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			// debug break point
			if(IsSelectedDebugEntity(entity))
				int a = 4;

			EntityState& state = GetComponentRef(EntityState, entity);

			// default
			if(state.current == Action::None && state.next == Action::None)
			{
				state.next = Action::Inactive;
			}

			bool must_finish_anim = 
				state.current == Action::AttackWindUp || 
				state.current == Action::BasicAttack || 
				state.current == Action::AttackRecovery;

			bool can_change_state = !must_finish_anim;
			if(must_finish_anim)
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