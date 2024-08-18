#pragma once
#include "pch.h"
#include "PhysicsSystem.h"

#include "ECS/Components/Physics.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"

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
				VectorF ray_start = collider->GetRect().BotCenter();
				VectorF direction = VectorF(0.0f, 1.0f);

				std::vector<Entity> self;
				self.push_back(entity);
				
				RaycastResult result;
				Raycast(ray_start, direction, 2.0f, result, &self);

				physics.onFloor = result.entity != EntityInvalid;
			}
			 
			if(physics.applyGravity && !physics.onFloor)
			{
				float multiplyer = 1.0f;
				
				if(ECS::CharacterState* character_state = ecs->GetComponent(CharacterState, entity))
				{
					if(character_state->actions.Top().action == ActionState::Jump)
					{
						multiplyer = 5.0f;

						if(physics.speed.y > 0.0f)
							multiplyer *= 1.5f;
					}
				}
				
				physics.speed += VectorF(0.0f, 9.8f) * multiplyer * dt;
				physics.speed = physics.speed.clamp(physics.maxSpeed * -1.0f, physics.maxSpeed);
			}


		}
	}
}
