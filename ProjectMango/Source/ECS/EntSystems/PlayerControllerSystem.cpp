#include "pch.h"
#include "PlayerControllerSystem.h"

#include "Characters/Spawner.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "Characters/Player/PlayerCharacter.h"

namespace ECS
{
	void PlayerControllerSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		InputManager* input = InputManager::Get();

		std::vector<ECS::Entity> to_respawn;

		for (Entity entity : entities)
		{
			PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
			CharacterState& state = ecs->GetComponentRef(CharacterState, entity);

			if(state.actions.HasAction())
			{
				CharacterAction* character_state = &state.actions.Top();
				character_state->Update(dt);

				if( character_state->action == ActionState::Death )
				{
					Player::DeathState* death_state = static_cast<Player::DeathState*>(character_state);
					if(death_state->can_respawn)
					{
						//respawn = true;
						to_respawn.push_back(entity);
						Player::Spawn();
						return;
					}
				}
				else
				{
					if(Health* health = ecs->GetComponent(Health, entity))
					{
						if(health->currentHealth <= 0.0f)
						{
							state.actions.Pop();
							state.actions.Push( new Player::DeathState(entity) );
						}
					}
				}
			}
			else
			{
				state.actions.Push( new Player::IdleState(entity) );
			}

			// Movement Direction
			const int horizontal_direction = input->isHeld(Button::Right) - input->isHeld(Button::Left);
			//const int vertical_direction = input->isHeld(Button::Up) || input->isHeld(Button::Down);

			state.movementInput = VectorI(horizontal_direction, 0);

			ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, entity);
			if (sprite.canFlip)
			{
				if (state.movementInput.x > 0)
					sprite.flip = SDL_FLIP_NONE;
				else if (state.movementInput.x < 0)
					sprite.flip = SDL_FLIP_HORIZONTAL;
			}
		}

		// kill entity with edit the entity list so do it outside the loop
		for( u32 i = 0; i < to_respawn.size(); i++ )
		{
			//ecs->entities.KillEntity(entities.front());
			//Player::Spawn();
		}
		to_respawn.clear();
	} 
}

