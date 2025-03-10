#include "pch.h"
#include "AIControllerSystem.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"

#include "Animations/CharacterStates.h"
#include "Entities/States/CharacterAction.h"
#include "Entities/Player/PlayerCharacter.h"
#include "System/Files/ConfigManager.h"

namespace ECS
{
	void AIControllerSystem::Update(float dt)
	{
		//
		const FrameRateController& frc = FrameRateController::Get();

		std::vector<Entity> dead_entities;

 		for (Entity entity : entities)
		{
			AIController& aic = GetComponentRef(AIController, entity);
			CharacterState& state = GetComponentRef(CharacterState, entity);

			if(state.actions.HasAction())
			{
				CharacterAction* character_state = &state.actions.Top();
				character_state->Update(dt);

				// so we dont add another death state when we already have one
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
						if(state.character->FinishedDying(entity))
						{
							dead_entities.push_back(entity);
							continue;
						}
					}
				}
				else
				{
					if(Health* health = GetComponent(Health, entity))
					{
						if(health->currentHealth <= 0.0f)
						{
							state.character->StartDying(entity);
						}
					}
				}
			}
			else
			{
				state.character->Begin(entity);
			}

			aic.target = Player::Get();

			// reset every frame
			aic.canMoveToTarget = false;

			if (HasComponent(Pathing, entity))
			{
				if (!ecs->IsAlive(aic.target))
					continue;

				const Config* config = GetConfig(entity);
				const float detect_range = config->data.GetFloat("alert_range");
				const VectorF distance = GetPosition(entity) - GetPosition(aic.target);
				const float target_distance = distance.length();

				if (target_distance < detect_range)
				{
					// try to flip to face the target direction
					SDL_RendererFlip desired_flip = GetDesiredFacingDirection(entity, aic.target);
					SetFacingDirection(entity, desired_flip);

					bool is_facing_target = GetDesiredFacingDirection(entity, aic.target) == GetFacingDirection(entity);

					// if we're facing the correct direction
					if (is_facing_target)
					{
						aic.canMoveToTarget = true;
					}
				}
			}
		}

		for( u32 i = 0; i < dead_entities.size(); i++ )
		{
			ecs->entities.KillEntity(dead_entities[i]);
		}
	}
}