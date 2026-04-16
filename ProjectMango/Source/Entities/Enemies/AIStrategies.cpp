#include "pch.h"
#include "AIStrategies.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntSystems/EntityStateSystem.h"
#include "Game/FrameRateController.h"

namespace AIStrategies
{
	using namespace ECS;

	static void ProgressStrategy(AIStrategy& strategy, TurnState* turn, bool force_phase_change)
	{
		// move to the next phase
		if(strategy.turnsLeft <= 0 || force_phase_change)
		{
			strategy.NextPhase();
		}
		
		// decrement turns AFTER
		strategy.turnsLeft--;

		if(turn)
			turn->tryEndTurn = true;
	}

	static void ProgressAttackPattern(AIStrategy& strategy, TurnState* turn, bool force_phase_change)
	{
		// move to the next phase
		if(strategy.turnsLeft <= 0 || force_phase_change)
		{
			strategy.NextAttackPattern();
		}
		
		// decrement turns AFTER
		strategy.turnsLeft--;

		if(turn)
			turn->tryEndTurn = true;
	}

	static float GetAttackRange(Entity entity, Action::Enum attack)
	{
		if(const BehaviourState* b_state = GetComponent(BehaviourState, entity))
		{
			if(b_state->attackData.contains(attack))
			{
				const AttackStateData& asd = b_state->attackData.at(attack);

				const Transform& transform = GetComponentRef(Transform, entity);
				const VectorF pos =  transform.worldPosition + transform.size * asd.hitBoxPos;
				const VectorF size = transform.size * asd.hitBoxSize;
				const RectF collider_rect(pos, size);

				const VectorF position = transform.GetObjectCenter();
				const float distance = Maths::Max( std::abs(position.x - collider_rect.RightCenter().x), std::abs(position.x - collider_rect.LeftCenter().x) );
				return distance;
			}
		}

		return -1.0f;
	}

	static bool WithinAttackRange(Entity entity, Entity target, Action::Enum attack)
	{
		const RectF target_rect = GetRect(target);
		const VectorF position = GetPosition(entity);

		// distance to the target is from the center of the entity 
		// to whichever side of the target is closer
		float distance_a = target_rect.LeftPoint() - position.x;
		float distance_b = target_rect.RightPoint() - position.x;
		float target_distance = Maths::Min( std::abs(distance_a), std::abs(distance_b) );

		return target_distance < (GetAttackRange(entity, attack) * 0.8f);
	}

	void SimpleAttacker(Entity entity, AIIntent& intent)
	{
		// grab the target if it has one
		Entity target = Faction::GetTarget(entity);

		if(target != EntityInvalid)
		{
			bool finished_spawning = false;
			if(Sprite* sprite = GetComponent(Sprite, entity))
			{
				finished_spawning = sprite->params.colourMod.a >= c_alphaMax * 0.95f;
				if(!finished_spawning)
					return;
			}

			intent.wantsToFaceTarget = true;

			if( WithinAttackRange(entity, target, Action::BasicAttack) )
			{
				intent.wantsToAttack = true;
			}

			if(!intent.wantsToAttack)
			{
				intent.wantsToMove = true;
			}
		}
	}

	void ShockSweeper(ECS::Entity entity, ECS::AIIntent& intent)
	{
		intent.wantsToFaceTarget = true;

		AIStrategy& strategy = GetComponentRef(AIStrategy, entity);
		TurnState* turn = GetComponent(TurnState, entity);

		EnemyPhase& phase = strategy.GetCurrentPhase();

		switch( phase.type )
		{	
			case EnemyPhase::Approach:
			{
				Entity target = Faction::GetTarget(entity);
				if( WithinAttackRange(entity, target, Action::BasicAttack) )
				{
					ProgressAttackPattern(strategy, turn, true);
					return;
				}
						
				intent.wantsToMove = true;
				break;
			}
			case EnemyPhase::Attack:
			{
				if( FinishedAttacking(entity) )
				{
					ProgressStrategy(strategy, turn, false);
					return;
				}

				intent.wantsToAttack = true;
				break;
			}
			case EnemyPhase::Recover:
			{	
				// push this state into the backlog, we need to seperate out the
				// idle phases so we know when we started/finished attacking
				EntityState& state = GetComponentRef(EntityState, entity);
				state.pushStateToBacklog = true;

				ProgressStrategy(strategy, turn, false);
				break;
			}
			default:
				break;
		}
	}
}