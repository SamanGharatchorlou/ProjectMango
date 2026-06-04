#include "pch.h"
#include "TransformSystem.h"

#include "ECS/Components/SpacialComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"

namespace ECS
{
	void TransformSystem::Update(float dt)
	{
		std::vector<Entity> out_of_bounds_entities;
		const Biome* biome = GetOnlyComponent(Biome);
		if (!biome)
			return;

		for (Entity entity : entities)
		{
			// debug break point
			if (IsSelectedDebugEntity(entity))
				int a = 4;

			Transform& transform = GetComponentRef(Transform, entity);

			// 3.
			// -- UPDATE POSITION
			// only move to the allowed position, otherwise roll back
			Collider* collider = GetComponent(Collider, entity);
			if (collider)
			{ 
				if(!collider->initialised)
				{
					DebugPrint(Warning, "Collder on entity %s has not been initialised", ECS::GetName(entity));
				}

				transform.worldPosition = transform.worldPosition + collider->allowedMovement;
			}
			// no collider so move it to its target position
			else
			{
				transform.worldPosition = transform.targetWorldPosition;
			}
			
			// 4.
			transform.UpdateChildTransforms();

			// check out of bounds
			const RectF rect = transform.GetRect();
			if (rect.RightPoint() < 0.0f || rect.BotPoint() < 0.0f || 
				rect.LeftPoint() > biome->size.x || rect.TopPoint() > biome->size.y)
			{
				out_of_bounds_entities.push_back(entity);
			}
		}

		for (u32 i = 0; i < out_of_bounds_entities.size(); i++)
		{
			ecs->entities.KillEntity(out_of_bounds_entities[i]);
		}
	}
}