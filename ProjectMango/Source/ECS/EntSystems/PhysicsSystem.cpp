#pragma once
#include "pch.h"
#include "PhysicsSystem.h"

#include "ECS/Components/Physics.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"
#include "Animations/CharacterStates.h"

#include "Graphics/Raycast.h"

namespace ECS
{
	void PhysicsSystem::Update(float dt)  
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		for (Entity entity : entities)
		{
			Physics& physics = ecs->GetComponentRef(Physics, entity);

			// todo what to use here, the sprite might be better
			if(const Collider* collider = ecs->GetComponent(Collider, entity))
			{
				VectorF ray_start = collider->rect.BotCenter();
				VectorF direction = VectorF(0.0f, 1.0f);

				std::vector<Entity> self;
				self.push_back(entity);
				
				std::vector<u32> collider_flags;
				collider_flags.push_back(ECS::Collider::IsTerrain);

				RaycastResult result;
				Raycast(ray_start, direction, 2.0f, result, &self, &collider_flags);

				physics.onFloor = result.entity != EntityInvalid;
								
				if(CharacterState* state = ecs->GetComponent(CharacterState, entity))
				{
					if(state->actions.HasAction() && state->actions.Top().action == ActionState::Jump)
					{
						physics.onFloor = false;
					}
				}
			}
			 
			if(physics.applyGravity)
			{
				if(!physics.onFloor)
				{
					float multiplyer = 200.0;

					if(physics.speed.y > 0.0f)
						multiplyer *= 2.2f;
				
					physics.speed += VectorF(0.0f, 9.8) * multiplyer * dt;
				}
				else
				{
					physics.speed.y = 0.0f;
				}

			}
		}
	}
}
