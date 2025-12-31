#include "pch.h"
#include "StateResolutionSystem.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/AIComponents.h"
#include "Core/Helpers.h"

namespace ECS
{
	// reads intent and set the next state
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

			bool coolingdown_from_attack = (b_state.attackFinishedTimeMS + b_state.attackCooldownTimeMS) > GetTicksMS();
			if(coolingdown_from_attack)
			{
				can_attack = false;
				can_move = false;
			}

			state.next = intent.wantsToBeInactive ? Action::Inactive : Action::Idle;

			if(Health* health = GetComponent(Health, entity))
			{
				if(health->currentHealth <= 0)
					state.next = Action::Death;
				else if( (health->currentHealth / health->maxHealth) <= 0.2f)
					state.next = Action::Hurting;
			}
			
			// death
			if(DeathScentence* death = GetComponent(DeathScentence, entity))
			{
				if(death->CanDie())
					state.next = Action::Death;
				
				// else waiting to die
				continue;
			}

			if(intent.wantsToAttack && can_attack)
			{
				if(state.current != Action::AttackWindUp && b_state.attackData.contains(Action::AttackWindUp))
				{
					state.next = Action::AttackWindUp;
				}
				else
				{
					state.next = Action::BasicAttack;
				}
			}
			else if(intent.wantsToMove && can_move)
			{
				state.next = Action::Run;
			}
		}
	}
}