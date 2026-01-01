#include "pch.h"
#include "StateResolutionSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
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

			// this breaks the attack recovery
			//bool coolingdown_from_attack = (b_state.attackFinishedTimeMS + b_state.attackCooldownTimeMS) > GetTicksMS();
			//if(coolingdown_from_attack)
			//{
			//	can_attack = false;
			//	can_move = false;
			//}

			Action::Enum& next_state = state.next;
			next_state = intent.wantsToBeInactive ? Action::Inactive : Action::Idle;

			if(Health* health = GetComponent(Health, entity))
			{
				if(health->currentHealth <= 0)
					next_state = Action::Death;
				else if( (health->currentHealth / health->maxHealth) <= 0.2f)
					next_state = Action::Hurting;
			}

			DeathScentence* death = GetComponent(DeathScentence, entity);
			bool waiting_to_die = death != nullptr;
			
			if(intent.wantsToAttack && can_attack)
			{
				// can only start a new attack once we complete an attack and 
				// we're not about to die, however we can finish an attack
				bool can_begin_new_attack = 
					state.current != Action::AttackWindUp && 
					state.current != Action::BasicAttack && 
					state.current != Action::AttackRecovery &&
					!waiting_to_die;

				if(can_begin_new_attack)
				{
					const Animator& animator = GetComponentRef(Animator, entity);
					bool has_attack_wind_up = animator.HasAnimation(Action::AttackWindUp);
					if(has_attack_wind_up)
						next_state = Action::AttackWindUp;
					else
						next_state = Action::BasicAttack;

					// starting a new attack, cant be inturrupted now
					//continue;
				}
				else
				{
					if(state.current == Action::AttackWindUp)
					{
						next_state = Action::BasicAttack;

						// finished wind up, starting actual attack
						//continue;
					}
					else if(state.current == Action::BasicAttack)
					{		
						const Animator& animator = GetComponentRef(Animator, entity);			
						bool has_attack_recovery = animator.HasAnimation(Action::AttackRecovery);
						if(has_attack_recovery)
						{
							next_state = Action::AttackRecovery;

							// finished attack, beginning recovery
							//continue;
						}
					}
				}
			}
			else if(intent.wantsToMove && can_move)
			{
				next_state = Action::Run;
			}
						
			// death
			if(waiting_to_die)
			{
				// actually die
				if(death->CanDie())
					next_state = Action::Death;
				
				// else waiting to die
				continue;
			}
		}
	}
}