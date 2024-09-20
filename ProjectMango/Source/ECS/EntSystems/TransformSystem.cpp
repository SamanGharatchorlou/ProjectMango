#include "pch.h"
#include "TransformSystem.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Physics.h"
#include "Core/Helpers.h"
#include "ECS/Components/Biome.h"

namespace ECS
{
	void TransformSystem::UpdateChildrenTransforms(Entity parent)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		Transform& parent_transform = ecs->GetComponentRef(Transform, parent);

		// update children positions
		if (EntityData* entity_data = ecs->GetComponent(EntityData, parent))
		{
			VectorF flip_point;
			if (ECS::Sprite* sprite = ecs->GetComponent(Sprite, parent))
			{
				if (sprite->IsFlipped())
					flip_point = sprite->flipPoint * parent_transform.size;
			}

			// handle horizontal flip 
			for (u32 i = 0; i < entity_data->children.size(); i++)
			{
				Entity child = entity_data->children[i];
				Transform& child_transform = ecs->GetComponentRef(Transform, child);
				VectorF child_world_pos = parent_transform.worldPosition + child_transform.localPosition;

				if (!flip_point.isZero())
				{
					VectorF flip_distance = child_transform.localPosition - flip_point;
					flip_distance.y = 0;
					child_world_pos -= (flip_distance * 2.0f + VectorF(child_transform.size.x, 0.0f));
				}

				child_transform.SetWorldPosition(child_world_pos);
			}
		}
	}

	void TransformSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		std::vector<Entity> out_of_bounds_entities;

		for (Entity entity : entities)
		{
			// debug break point
			if (DebugMenu::GetSelectedEntity() == entity)
				int a = 4;

			Transform& transform = ecs->GetComponentRef(Transform, entity);

			// only move to the allowed position, otherwise roll back
			Collider* collider = ecs->GetComponent(Collider, entity);
			if (collider)
			{
				u32 flags = Collider::Flags::Static;
				if (HasFlag(collider->flags, flags))
					continue;

				transform.worldPosition = transform.worldPosition + collider->allowedMovement;

				UpdateChildrenTransforms(entity);
			}

			// update the target position based on physics
			Physics* physics = ecs->GetComponent(Physics, entity);
			if (physics)
			{
				transform.targetWorldPosition = transform.worldPosition + (physics->speed * dt);
			}

			// set collider paramters
			if (collider)
			{
				collider->back = transform.worldPosition;
				collider->forward = transform.targetWorldPosition;
				collider->RollForwardPosition();
			}

			// check out of bounds
			if (const Biome* biome = Biome::GetActiveBiome())
			{
				const VectorF position = transform.GetObjectCenter();
				if (position.x < biome->aabb[0].x || position.y < biome->aabb[0].y || 
					position.x > biome->aabb[1].x || position.y > biome->aabb[1].y)
				{
					out_of_bounds_entities.push_back(entity);
				}
			}
		}

		for (u32 i = 0; i < out_of_bounds_entities.size(); i++)
		{
			ecs->entities.KillEntity(out_of_bounds_entities[i]);
		}
	}
}