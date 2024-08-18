#include "pch.h"
#include "TransformSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Physics.h"
#include "Core/Helpers.h"

namespace ECS
{
	void TransformSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

 		for (Entity entity : entities)
		{
			Transform& transform = ecs->GetComponentRef(Transform, entity);

			// only move to the allowed position, otherwise roll back
			Collider* collider = ecs->GetComponent(Collider, entity);
			if(collider)
			{
				u32 flags = Collider::Flags::Static;
				if(HasFlag(collider->mFlags, flags))
					continue;

				transform.position = transform.position + collider->allowedMovement;
			}

			// update the target position based on physics
			Physics* physics = ecs->GetComponent(Physics, entity);
			if(physics)
			{
				transform.targetPosition = transform.position + physics->speed;
			}

			// set collider paramters
			if(collider)
			{
				collider->mBack = transform.position;
				collider->mForward = transform.targetPosition;
				collider->RollForwardPosition();

				if( physics )
				{
					if(collider->allowedMovement.x == 0)
						physics->speed.x = 0;

					if(collider->allowedMovement.y == 0)
						physics->speed.y = 0;
				}
			}
		}
	}
}