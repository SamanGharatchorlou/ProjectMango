#include "pch.h"
#include "StateResolutionSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Core/Helpers.h"

namespace ECS
{
	// reads intent and set the next state - the desire
	// "what should this entity be trying to do?"
	void StateResolutionSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			// debug break point
			if(IsSelectedDebugEntity(entity))
				int a = 4;

			EntityState& state = GetComponentRef(EntityState, entity);
			const AIIntent& intent = GetComponentRef(AIIntent, entity);
			const BehaviourState& b_state = GetComponentRef(BehaviourState, entity);
			
			if(intent.wantsToFaceTarget)
			{
				// try to flip to face the target direction
				Entity target = Faction::GetTarget(entity);
				if(target != EntityInvalid)
				{
					SDL_RendererFlip desired_flip = GetDesiredFacingDirection(entity, target);
					SetFacingDirection(entity, desired_flip);
				}
			}

			bool can_attack = true;
			bool can_move = true;
			if(AIController* ai = GetComponent(AIController, entity))
			{
				if(ai->isDisabled)
				{
					can_attack = false;
					can_move = false;
				}
			}

			Action::Enum& next_state = state.next;
			next_state = intent.wantsToBeInactive ? Action::Inactive : Action::Idle;
			
			// waiting to die
			DeathScentence* death = GetComponent(DeathScentence, entity);
			bool waiting_to_die = death != nullptr;
			if(waiting_to_die)
			{
				// actually die
				if(death->CanDie())
					next_state = Action::Death;
				
				// else waiting to die
				continue;
			}

			// attack
			if(intent.wantsToAttack && can_attack)
			{
				next_state = Action::AttackWindUp;
			}
			else if(intent.wantsToDebuff && can_attack)
			{
				next_state = Action::Debuff;
			}
			// move
			else if(intent.wantsToMove && can_move)
			{
				next_state = Action::Run;
			}
						
			// getting hurt or dying
			if (Health* health = GetComponent(Health, entity))
			{
				if (health->currentHealth <= 0)
					next_state = Action::Death;
			}
		}
	}
}