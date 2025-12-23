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
		const Transform& parent_transform = GetComponentRef(Transform, parent);

		// update children positions
		if (EntityData* entity_data = GetComponent(EntityData, parent))
		{
			SDL_RendererFlip sprite_flip = SDL_FLIP_NONE;

			VectorF flip_point;
			if (ECS::Sprite* parent_sprite = GetComponent(Sprite, parent))
			{
				if (parent_sprite->IsFlipped())
				{
					flip_point = parent_transform.GetHorizontalFlipPoint();
					sprite_flip = parent_sprite->flip;
				}
			}

			// handle horizontal flip 
			for (u32 i = 0; i < entity_data->children.size(); i++)
			{
				Entity child = entity_data->children[i];
				Transform& child_transform = GetComponentRef(Transform, child);
				VectorF child_world_pos = parent_transform.worldPosition + child_transform.localPosition;

				if (!flip_point.isZero())
				{
					VectorF flip_distance = child_transform.localPosition - flip_point;
					flip_distance.y = 0;
					child_world_pos -= (flip_distance * 2.0f + VectorF(child_transform.size.x, 0.0f));
				}

				if (ECS::Sprite* sprite = GetComponent(Sprite, child))
				{
					if (sprite->canFlip)
					{
						bool was_flipped = sprite->IsFlipped();

						sprite->flip = sprite_flip;
						
						bool is_flipped = sprite->IsFlipped();

						if(was_flipped != is_flipped)
						{
							int a = 4;
						}

					}
				}

				child_transform.SetWorldPosition(child_world_pos);

			}
		}
	}

	void TransformSystem::Update(float dt)
	{
		std::vector<Entity> out_of_bounds_entities;

		for (Entity entity : entities)
		{
			// debug break point
			if (DebugMenu::GetSelectedEntity() == entity)
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
			UpdateChildrenTransforms(entity);

			// check out of bounds
			const Biome& biome = Biome::GetActiveBiome();
			const RectF rect = transform.GetRect();
			if (rect.RightPoint() < biome.aabb[0].x || rect.BotPoint() < biome.aabb[0].y || 
				rect.LeftPoint() > biome.aabb[1].x || rect.TopPoint() > biome.aabb[1].y)
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