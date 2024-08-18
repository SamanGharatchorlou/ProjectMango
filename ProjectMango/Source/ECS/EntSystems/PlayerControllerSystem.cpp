#include "pch.h"
#include "PlayerControllerSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Input/InputManager.h"
#include "ECS/Components/PlayerController.h"
#include "ECS/Components/Physics.h"

#include "Characters/Spawner.h"
#include "Characters/Player/PlayerCharacter.h"
#include "Core/Helpers.h"

#include "Debugging/ImGui/Components/ComponentDebugMenu.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

#include "ECS/EntSystems/CollisionSystem.h"

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
			Physics& physics = ecs->GetComponentRef(Physics, entity);

			bool respawn_player = false;

			if(state.actions.HasAction())
			{
				CharacterAction* character_state = &state.actions.Top();
				character_state->Update(dt);

				//if(ECS::Collider* collider = ecs->GetComponent(Collider, entity))
				//{
				//	if(HasFlag(collider->mFlags, Collider::IgnoreCollisions))
				//	{
				//		// remove the flag so we can make a "would it collide" check
				//		// if so, add it back we'll end up in a bad state
				//		RemoveFlag(collider->mFlags, (u32)Collider::IgnoreCollisions);
				//		if(CollisionSystem::DoesColliderInteract(entity))
				//		{
				//			SetFlag(collider->mFlags, (u32)Collider::IgnoreCollisions);
				//		}
				//	}
				//}

				if( character_state->action == ActionState::Death )
				{
					Player::DeathState* death_state = static_cast<Player::DeathState*>(character_state);
					if(death_state->can_respawn)
					{
						//respawn = true;
						to_respawn.push_back(entity);
						continue;
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

			//// Movement Direction
			//pc.hasMovementInput = input->isHeld(Button::Right) || input->isHeld(Button::Left) || 
			//						input->isHeld(Button::Up) || input->isHeld(Button::Down);

			int horizontal_direction = input->isHeld(Button::Right) - input->isHeld(Button::Left);
			int vertical_direction = 0;

			state.movementInput = VectorI(horizontal_direction, vertical_direction);
		}

		for( u32 i = 0; i < to_respawn.size(); i++ )
		{
			ecs->entities.KillEntity(entities.front());
			PlayerSpawn::Spawn();
		}
		to_respawn.clear();
	} 
}

