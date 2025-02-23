#include "pch.h"
#include "PlayerControllerSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "Entities/Player/PlayerCharacter.h"
#include "Entities/States/PlayerMeleeStates.h"
#include "Entities/States/PlayerRangedStates.h"
#include "Animations/CharacterStates.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Biome.h"
#include "Game/Camera/Camera.h"

#include "ECS/Components/ComponentCommon.h"

namespace ECS
{
	static bool s_instantFirstSpawn = true;

	static void SpawnPlayer()
	{
		

		ComponentArray<Spawner>& spawners = GetAllComponents(Spawner);

		std::vector<Entity> out_spawners;
		GetEntitiesInLevel(Biome::GetVisibleLevel(), spawners.entityToComponent, out_spawners);

		Spawner* spawner = nullptr;
		if(out_spawners.size() > 0)
		{
			spawner = GetComponent(Spawner, out_spawners.front());
		}
		if(!spawner && out_spawners.size() > 0)
		{
			spawner = GetComponent(Spawner, out_spawners.front());
		}

		if(spawner && !spawner->IsSpawning())
		{
			spawner->Spawn("Player", Player::Spawn);

			VectorF spawner_center = GetPosition(spawner->entity);

			Camera* camera = Camera::Get();
			camera->targetEntity = spawner->entity;
		}
	}

	void PlayerControllerSystem::Update(float dt)
	{
		
		InputManager* input = InputManager::Get();

		for (Entity entity : entities)
		{
			PlayerController& pc = GetComponentRef(PlayerController, entity);
			CharacterState& state = GetComponentRef(CharacterState, entity);

			if(state.actions.HasAction())
			{
				CharacterAction* character_state = &state.actions.Top();
				character_state->Update(dt);

				if( character_state->action == ActionState::Death )
				{
					if(state.isMelee)
					{
  						PlayerMelee::DeathState* death_state = static_cast<PlayerMelee::DeathState*>(character_state);
						if(death_state->canRespawn)
						{
							SpawnPlayer();
							return;
						}
					}
					else
					{
  						PlayerRanged::DeathState* death_state = static_cast<PlayerRanged::DeathState*>(character_state);
						if(death_state->canRespawn)
						{
							SpawnPlayer();
							return;
						}
					}
				}
				else
				{
					if(Health* health = GetComponent(Health, entity))
					{
						if(health->currentHealth <= 0.0f)
						{
							state.actions.Pop();

							if(state.isMelee)
  								state.actions.Push( new PlayerMelee::DeathState(entity) );
							else
  								state.actions.Push( new PlayerRanged::DeathState(entity) );
						}
					}
				}

				const Physics& physics = GetComponentRef(Physics, entity);
				const Collider& collider = GetComponentRef(Collider, entity);
				bool not_moving_upwards = physics.speed.y > 0.0f|| collider.collisionSide[Collider::Top];
				if(!physics.onFloor && not_moving_upwards)
				{
					if( character_state->action != ActionState::Fall && character_state->action != ActionState::FloorSlam )
					{
						if(state.isMelee)
  							state.actions.Push( new PlayerMelee::FallState(entity) );
						else
  							state.actions.Push( new PlayerRanged::FallState(entity) );
					}
				}

				if(physics.onFloor)
					state.canEnterHover = true;
			}
			else
			{
				if(state.isMelee)
  					state.actions.Push( new PlayerMelee::IdleState(entity) );
				else
  					state.actions.Push( new PlayerRanged::IdleState(entity) );
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

			ECS::Sprite& sprite = GetComponentRef(Sprite, entity);
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

		if(deathTimer.GetSeconds() > 2.0f || s_instantFirstSpawn)
		{
			s_instantFirstSpawn = false;
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

