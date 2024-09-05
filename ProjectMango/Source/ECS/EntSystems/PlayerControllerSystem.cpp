#include "pch.h"
#include "PlayerControllerSystem.h"

#include "Characters/Spawner.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Characters/States/PlayerStates.h"
#include "Animations/CharacterStates.h"
#include "ECS/Components/Physics.h"

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

				if(Physics* physics = ecs->GetComponent(Physics, entity))
				{
					if(!physics->onFloor && physics->speed.y >= 0.0f)
					{
						if( character_state->action != ActionState::Fall )
						{
							state.actions.Push( new Player::FallState(entity) );
						}
					}
				}
			}
			else
			{
				state.actions.Push( new Player::IdleState(entity) );
			}

			// Movement Direction
			bool move_right = input->isHeld(Button::Right);
			bool move_left = input->isHeld(Button::Left);
			int horizontal_direction = move_right - move_left;

			if (move_right && move_left)
			{
				// take the most recently pressed direction
				if (input->getHeldFrames(Button::Right) < input->getHeldFrames(Button::Left))
					horizontal_direction = 1;
				else
					horizontal_direction = -1;
			}

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
	} 
}

