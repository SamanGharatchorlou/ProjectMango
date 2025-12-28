#pragma once
#include "pch.h"
#include "PhysicsSystem.h"

#include "ECS/Components/SpacialComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/Raycast.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

namespace ECS
{
	void PhysicsSystem::Update(float dt)  
	{
		for (Entity entity : entities)
		{			
			// debug break point
			if (DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			Physics& physics = GetComponentRef(Physics, entity);
			Transform& transform = GetComponentRef(Transform, entity);

			// todo what to use here, the sprite might be better
			RectF rect = transform.GetObjectRect();

			//VectorF direction = VectorF(0.0f, 1.0f);

			std::vector<Entity> self;
			self.push_back(entity);
				
			std::vector<u32> collider_flags;
			collider_flags.push_back(ECS::Collider::IsFloor);
				
			
			VectorF edge_buffer(rect.Width() * 0.15f, 0.0f);

			u32 ray_count = 2;
			VectorF start_rays[2] = { rect.BotLeft() + edge_buffer , rect.BotRight() - edge_buffer };
				
			physics.onFloor = false;
			for( u32 i = 0; i < ray_count; i++ )
			{
				RaycastResult result;
				Raycast(start_rays[i], VectorF(0,1.0f), 3.0f, result, &self, &collider_flags);
				if(result.hasHit)
				{
					physics.onFloor = true;
					break;
				}
			}
			 
			if(physics.applyGravity)
			{
				if(!physics.onFloor)
				{
					float multiplyer = 120.0;

					// returning to floor
					if(physics.speed.y > 0.0f)
						multiplyer *= 2.5f;
				
					physics.speed += VectorF(0.0f, 9.8f) * multiplyer * dt;
				}
				else
				{
					physics.speed.y = 0.0f;
				}
			}

			physics.speed.x += physics.acceleration * dt;
			physics.speed.x = Maths::clamp(physics.speed.x, -physics.maxSpeed, physics.maxSpeed);
			
			if( physics.acceleration == 0.0f)
			{
				physics.speed.x = physics.speed.x * (1 - physics.drag * dt);

				const float min_speed = 0.01f;
				if ( std::abs(physics.speed.x) < min_speed )
					physics.speed.x = 0;
			}

			// move target
			transform.targetWorldPosition = transform.worldPosition + (physics.speed * dt);
		}
	}
}
