#include "pch.h"
#include "AIControllerSystem.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Physics.h"
#include "ECS/EntityCoordinator.h"
#include "Game/FrameRateController.h"

#include "Animations/CharacterStates.h"
#include "Characters/States/CharacterAction.h"
#include "Characters/Player/PlayerCharacter.h"
#include "System/Files/ConfigManager.h"

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
					if(Health* health = ecs->GetComponent(Health, entity))
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

			const ObjectConfig* config = GetObjectConfig(entity);
			const VectorF detect_range = config->values.GetVectorF( "target_detect_range" );
			const VectorF distance = GetPosition(entity) - GetPosition(aic.target);
			aic.isAlert = detect_range.x < distance.x && detect_range.y < distance.y;


		}

		for( u32 i = 0; i < dead_entities.size(); i++ )
		{
			ecs->entities.KillEntity(dead_entities[i]);
		}
	}
}