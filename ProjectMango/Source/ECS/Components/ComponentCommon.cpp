#include "pch.h"
#include "ComponentCommon.h"

#include "ECS/EntityCoordinator.h"
#include "Components.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Biome.h"

namespace ECS
{
	
	void FilterEntitiesInLevel(const ECS::Level& level, const std::vector<ECS::Entity>& in_entities, std::vector<ECS::Entity>& out_entities)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		for( u32 i = 0; i < in_entities.size(); i++ )
		{
			if(const Transform* transform = ecs->GetComponent(Transform, in_entities[i]))
			{
				const VectorF position = transform->worldPosition;
				const VectorF world_pos = level.worldPos;

				if(position.x > world_pos.x && position.y > world_pos.y)
				{
					const VectorF world_pos_end = world_pos + level.size;
					if(position.x < world_pos_end.x && position.y < world_pos_end.y)
					{
						out_entities.push_back(in_entities[i]);
					}
				}
			}
		}
	}
	
	void GetAllComponentsInLevel(ECS::Level& level)
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;

		std::vector<ECS::Entity> out_entities;

		std::vector<ECS::Entity> entities;
		ecs->GetEntitiesWithComponent(Collider, entities);

		for( u32 i = 0; i < entities.size(); i++ )
		{
			if(const Transform* transform = ecs->GetComponent(Transform, entities[i]))
			{
				const VectorF position = transform->worldPosition;
				const VectorF world_pos = level.worldPos;

				if(position.x > world_pos.x && position.y > world_pos.y)
				{
					const VectorF world_pos_end = world_pos + level.size;
					if(position.x < world_pos_end.x && position.y < world_pos_end.y)
					{
						out_entities.push_back(entities[i]);
					}
				}
			}
		}
	}
}