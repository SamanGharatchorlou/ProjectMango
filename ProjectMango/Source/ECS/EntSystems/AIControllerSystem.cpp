#include "pch.h"
#include "AIControllerSystem.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"

#include "Characters/Player/PlayerCharacter.h"

bool EnemyCanMove();

namespace ECS
{
	void AIControllerSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		const FrameRateController& frc = FrameRateController::Get();

		std::vector<Entity> dead_entities;

 		for (Entity entity : entities)
		{
			AIController& aic = ecs->GetComponentRef(AIController, entity);
			CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

			if(state.actions.HasAction())
			{
				CharacterAction* character_state = &state.actions.Top();
				character_state->Update(dt);

				bool is_dying = false;
				for( u32 i = 0; i < state.actions.stack.size(); i++ )
				{
					if(state.actions.stack[i]->action == ActionState::Death)
						is_dying = true;
				}

				if( is_dying )
				{
					if(character_state->action == ActionState::Death)
					{
						Enemy::DeathState* death_state = static_cast<Enemy::DeathState*>(character_state);
						if(death_state->can_kill)
						{
							dead_entities.push_back(entity);
							continue;
						}
					}
				}
				else
				{
					if(Health* health = ecs->GetComponent(Health, entity))
					{
						if(health->currentHealth <= 0.0f)
						{
							state.actions.Pop();
							state.actions.Push( new Enemy::DeathState(entity) );
							state.actions.Push( new Enemy::TakeHitState(entity) );
						}
					}
				}
			}
			else
			{
				state.actions.Push( new Enemy::IdleState(entity) );
			}

			aic.target = Player::Get();
		}

		//for( u32 i = 0; i < dead_entities.size(); i++ )
		//{
		//	ecs->entities.KillEntity(dead_entities[i]);
		//}
	}
}