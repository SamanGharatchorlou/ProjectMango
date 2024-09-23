#include "pch.h"
#include "PlayerControllerSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Characters/States/PlayerStates.h"
#include "Animations/CharacterStates.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Biome.h"
#include "Game/Camera/Camera.h"

#include "ECS/Components/ComponentCommon.h"
#include "Core/Helpers.h"

namespace ECS
{
	static void SpawnPlayer()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		std::vector<Entity> spawners;
		ecs->GetEntitiesWithComponent(Spawner, spawners);

		std::vector<Entity> out_spawners;

		const Level& active_level = Biome::GetVisibleLevel();
		FilterEntitiesInLevel(active_level, spawners, out_spawners);

		Spawner* spawner = nullptr;
		if(out_spawners.size() > 0)
		{
			spawner = ecs->GetComponent(Spawner, out_spawners.front());
		}
		if(!spawner && spawners.size() > 0)
		{
			spawner = ecs->GetComponent(Spawner, spawners.front());
		}

		if(spawner && !spawner->IsSpawning())
		{
			spawner->Spawn("Player", "PlayerDataConfig", Player::Spawn);

			VectorF spawner_center = GetPosition(spawner->entity);

			Camera* camera = Camera::Get();
			camera->targetEntity = spawner->entity;
		}
	}

	void PlayerControllerSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		InputManager* input = InputManager::Get();

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
					if(death_state->canRespawn)
					{
						SpawnPlayer();
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

				const Physics& physics = ecs->GetComponentRef(Physics, entity);
				const Collider& collider = ecs->GetComponentRef(Collider, entity);
				bool not_moving_upwards = physics.speed.y > 0.0f|| collider.collisionSide[Collider::Top];
				if(!physics.onFloor && not_moving_upwards)
				{
					if( character_state->action != ActionState::Fall && character_state->action != ActionState::FloorSlam )
					{
  						state.actions.Push( new Player::FallState(entity) );
					}
				}

				if(physics.onFloor)
					pc.canEnterHover = true;
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

		if (entities.size() == 0 && !deathTimer.IsRunning())
		{
			deathTimer.Start();
		}

		if(deathTimer.GetSeconds() > 2.0f)
		{
			if(!spawningPlayer)
			{
				SpawnPlayer();
				spawningPlayer = true;
			}
		}

		if(entities.size() > 0)
		{
			spawningPlayer = false;
			deathTimer.Stop();
		}
	} 
}

