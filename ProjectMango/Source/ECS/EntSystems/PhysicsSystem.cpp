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
				VectorF direction = VectorF(0.0f, 1.0f);

				std::vector<Entity> self;
				self.push_back(entity);
				
				std::vector<u32> collider_flags;
				collider_flags.push_back(ECS::Collider::IsTerrain);
				
				// todo:
				VectorF edge_buffer(collider->rect.Width() * 0.15f, 0.0f);

				u32 ray_count = 2;
				VectorF start_rays[2] = { collider->rect.BotLeft() + edge_buffer , collider->rect.BotRight() - edge_buffer };
				
				physics.onFloor = false;
				for( u32 i = 0; i < ray_count; i++ )
				{
					RaycastResult result;
					Raycast(start_rays[i], direction, 2.0f, result, &self, &collider_flags);
					physics.onFloor |= result.entity != EntityInvalid;

					if(physics.onFloor)
						break;
				}
								
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
