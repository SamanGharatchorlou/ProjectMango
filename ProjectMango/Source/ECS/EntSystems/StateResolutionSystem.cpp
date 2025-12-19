#include "pch.h"
#include "StateResolutionSystem.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/AIComponents.h"

namespace ECS
{
	// reads intent and set the next state
	void StateResolutionSystem::Update(float dt)
	{
 		for (Entity entity : entities)
		{
			EntityState& state = GetComponentRef(EntityState, entity);
			const AIIntent& intent = GetComponentRef(AIIntent, entity);
			BehaviourState* behaviour_state = GetComponent(BehaviourState, entity);
			
			if(intent.wantsToFaceTarget)
			{
				// try to flip to face the target direction
				
				Entity target = Target::GetValidTarget(entity);
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

			if(behaviour_state)
			{
				bool coolingdown_from_attack = (behaviour_state->attackFinishedTimeMS + behaviour_state->attackCooldownTimeMS) > GetTicksMS();
				if(coolingdown_from_attack)
				{
					can_attack = false;
					can_move = false;
				}
			}

			state.next = Action::Idle;

			if(intent.wantsToAttack && can_attack)
			{
				state.next = Action::BasicAttack;
			}
			else if(intent.wantsToMove && can_move)
			{
				state.next = Action::Run;
			}
		}
	}
}