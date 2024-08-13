#include "pch.h"
#include "PlayerControllerSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/Components/Physics.h"

#include "Characters/Spawner.h"
#include "Characters/Player/PlayerCharacter.h"

#include "Debugging/ImGui/Components/ComponentDebugMenu.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

#include "ECS/EntSystems/CollisionSystem.h"

namespace ECS
{
	static void UpdatePlayerState(ECS::Entity entity, float dt, bool& respawn)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
		if(pc.actions.HasAction())
		{
			CharacterAction* character_state = &pc.actions.Top();
			character_state->Update(dt);

			if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
			{
				if(HasFlag(collider->mFlags, Collider::IgnoreCollisions))
				{
					// remove the flag so we can make a "would it collide" check
					// if so, add it back we'll end up in a bad state
					RemoveFlag(collider->mFlags, (u32)Collider::IgnoreCollisions);
					if(CollisionSystem::DoesColliderInteract(entity))
					{
						SetFlag(collider->mFlags, (u32)Collider::IgnoreCollisions);
					}
				}
			}

			if( character_state->action == ActionState::Death )
			{
				Player::DeathState* death_state = static_cast<Player::DeathState*>(character_state);
				if(death_state->can_respawn)
				{
					respawn = true;
				}
			}
			else
			{
				if(Health* health = ecs->GetComponent(Health, entity))
				{
					if(health->currentHealth <= 0.0f)
					{
						pc.PopState();
						pc.PushState(ActionState::Death);
					}
				}
			}
		}
		else
		{
			pc.PushState(ActionState::Idle);
		}
	}

	void PlayerControllerSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		InputManager* input = InputManager::Get();

		std::vector<ECS::Entity> to_respawn;

		for (Entity entity : entities)
		{
			PlayerController& pc = ecs->GetComponentRef(PlayerController, entity);
			CharacterState& state = ecs->GetComponentRef(CharacterState, entity);
			//Transform& transform = ecs->GetComponentRef(Transform, entity);
			Physics& physics = ecs->GetComponentRef(Physics, entity);

			bool using_playlist = DebugMenu::UsingPlaylist(entity);
			if(using_playlist)
				continue;

			// update the transform with where we wanted to move last frame 
			// it may have been changed by the collision system
			//transform.rect.SetCenter(transform.targetCenterPosition);

			bool respawn_player = false;
			UpdatePlayerState(entity, dt, respawn_player);

			if(respawn_player)
			{
				to_respawn.push_back(entity);
				continue;
			}

			//pc.actions.ProcessStateChanges();
			state.action = pc.actions.Top().action;
			 
			// Movement Direction
			pc.hasMovementInput = input->isHeld(Button::Right) || input->isHeld(Button::Left) || 
				input->isHeld(Button::Up) || input->isHeld(Button::Down);

			int horizontal_direction = input->isHeld(Button::Right) - input->isHeld(Button::Left);
			int vertical_direction = input->isHeld(Button::Down) - input->isHeld(Button::Up);

			state.movementDirection = VectorI(horizontal_direction, vertical_direction);

			// if we're moving use the speed so we always face the direction we're moving
			if(!physics.speed.isZero())
			{
				float x_mag = physics.speed.x * physics.speed.x;
				float y_mag = physics.speed.y * physics.speed.y;

				if(x_mag > y_mag)
				{
					state.facingDirection.x = physics.speed.x > 0.0f ? 1 : -1;
					state.facingDirection.y = 0;
				}
				else if(x_mag < y_mag)
				{
					state.facingDirection.y = physics.speed.y > 0.0f ? 1 : -1;
					state.facingDirection.x = 0;
				}
			}

			// Running
			state.isRunning = input->isHeld(Button::Shift);

			// where we're trying to move to
			//transform.targetCenterPosition = transform.rect.Translate(physics.speed).Center();
		}

		for( u32 i = 0; i < to_respawn.size(); i++ )
		{
			ecs->entities.KillEntity(entities.front());
			
			PlayerSpawn::Spawn();
			spawnPlayer = false;

		}

		to_respawn.clear();
	} 
}

