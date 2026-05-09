#include "pch.h"
#include "EntityStateSystem.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GraphicComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"

namespace ECS
{
	bool IsAttackState(Action::Enum state)
	{
		bool attack_action = 
			state == Action::AttackWindUp || 
			state == Action::BasicAttack || 
			state == Action::FollowUpAttack || 
			state == Action::AttackRecovery ||
			state == Action::Debuff;

		return attack_action;
	}

	static Action::Enum ResolveAttackChain(Entity entity, EntityState& state)
	{
		// not in an attack chain, just use whatever resolution set
		if(!IsAttackState(state.current))
			return state.next;
				
		const Animator& animator = GetComponentRef(Animator, entity);
		    
		// we're mid chain - progress it regardless of what next is
		switch(state.current)
		{
			case Action::AttackWindUp:
			{
				return Action::BasicAttack;
			}
			case Action::BasicAttack:
			{
				if( animator.HasAnimation(Action::FollowUpAttack) )
					return Action::FollowUpAttack;

				__fallthrough;
			}
			case Action::FollowUpAttack:
			{
				if(animator.HasAnimation(Action::AttackRecovery))
					return Action::AttackRecovery;

				__fallthrough;
			}
			case Action::AttackRecovery:
			{
				return Action::Idle;
			}
			case Action::Debuff:
			{
				return Action::Idle;
			}

			default:
				return state.next;
		}
	}

	bool CanChangeState(Entity entity)
	{
		EntityState& state = GetComponentRef(EntityState, entity);

		bool must_finish_anim = IsAttackState(state.current);

		bool can_change_state = !must_finish_anim;
		if(must_finish_anim)
		{
			if(Animator* animator = GetComponent(Animator, entity))
			{
				if(!animator->HasAnimation(state.current))
					return true;

				can_change_state = animator->loopCount > 0;
			}
		}

		return can_change_state;
	}

	bool FinishedAttacking(Entity entity)
	{
		EntityState& state = GetComponentRef(EntityState, entity);
		if(state.backlog.size() == 0)
			return false;

		Action::Enum last_action = state.backlog.back();
		return IsAttackState(last_action) && !IsAttackState(state.current);
	}

	// commits the state data i.e. applies the current state, respecting any blocks
	// "given what we're trying to do and what state we're currently in, what's the actual next state?"
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

				if(Animator* animator = GetComponent(Animator, entity))
				{
					if(!animator->HasAnimation(Action::Inactive))
					{
						if(animator->HasAnimation(Action::Idle))
							state.next = Action::Idle;
					}
				}
			}

			state.justChanged = false;


			if(CanChangeState(entity))
			{	
				// if we're in an attack chain that overrides the state as we must finish it
				state.next = ResolveAttackChain(entity, state);

				if(state.current != state.next)
				{
					state.backlog.push_back(state.current);
					state.current = state.next;

					state.justChanged = true;
				}
			}

			if(!state.justChanged && state.pushStateToBacklog)
			{
				state.backlog.push_back(state.current);
				state.pushStateToBacklog = false;
			}
		}
	}
}