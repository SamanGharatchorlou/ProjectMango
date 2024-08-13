#include "pch.h"
#include "AIControllerSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/AIController.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

#include "Characters/Player/PlayerCharacter.h"

bool EnemyCanMove();

namespace ECS
{
	void AIControllerSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		std::vector<Entity> dead_entities;

 		for (Entity entity : entities)
		{
			AIController& aic = ecs->GetComponentRef(AIController, entity);
			CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
			Physics& physics = ecs->GetComponentRef(Physics, entity);
			Transform& transform = ecs->GetComponentRef(Transform, entity);

			//transform.rect.SetCenter(transform.targetCenterPosition);
			
			if(aic.actions.HasAction())
			{
				CharacterAction* character_state = &aic.actions.Top();
				character_state->Update(dt);

				if( character_state->action == ActionState::Death )
				{
					Enemy::DeathState* death_state = static_cast<Enemy::DeathState*>(character_state);
					if(death_state->can_kill)
					{
						dead_entities.push_back(entity);
						continue;
					}
				}
				else
				{
					if(Health* health = ecs->GetComponent(Health, entity))
					{
						if(health->currentHealth <= 0.0f)
						{
							aic.PopState();
							aic.PushState(ActionState::Death);
						}
					}
				}
			}
			else
			{
				aic.PushState(ActionState::Idle);
			}

			// update the state actions
			state.action = aic.actions.Top().action;

			if(Pathing* pathing = ecs->GetComponent(Pathing, entity))
			{
				pathing->target = Player::Get();
				//pathing->UpdateTargetPosition();

				// 294, 366 -> 302 ,340
				// -8, 26

				// if we have a really small path length and we're not active
				// dont bother moving, this prevents bouncing between small paths
				// due to movement speeds overshooting targets
				bool too_lazy_to_move = false;
				if (state.action == ActionState::Idle)
				{
					too_lazy_to_move = pathing->path.size() <= 3;
				}

				if( pathing->path.size() > 1 && !too_lazy_to_move)
				{
					VectorI current = pathing->path.back();
					VectorI next = pathing->path[pathing->path.size() - 2];

					state.movementDirection = next - current;

					if(!state.movementDirection.isZero() && !physics.speed.isZero())
					{
						state.facingDirection = state.movementDirection;
					}
				}
				else
				{
					state.movementDirection = VectorI::zero();
					//physics.speed = VectorF::zero();
				}
			}
						
			if(DebugMenu::GetSelectedEntity() == entity)
			{
				if(!EnemyCanMove())
				{
					physics.speed = VectorF::zero();
				}
			}

			// where we're trying to move to
			//transform.targetCenterPosition = transform.rect.Translate(physics.speed).Center();
		}

		for( u32 i = 0; i < dead_entities.size(); i++ )
		{
			ecs->entities.KillEntity(dead_entities[i]);
		}
	}
}