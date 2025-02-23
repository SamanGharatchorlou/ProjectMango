#include "pch.h"
#include "ComponentCommon.h"

#include "ECS/EntityCoordinator.h"
#include "Components.h"
#include "ECS/Components/Biome.h"

namespace ECS
{
	bool IsInLevel(const Level& level, const Transform& transform)
	{
		const VectorF position = transform.worldPosition;
		const VectorF world_pos = level.worldPos;

		if (position.x > world_pos.x && position.y > world_pos.y)
		{
			const VectorF world_pos_end = world_pos + level.size;
			return position.x < world_pos_end.x && position.y < world_pos_end.y;
		}

		return false;
	}

	void GetEntitiesInLevel(const Level& level, const std::unordered_map<Entity, u32>& in_entities, std::vector<Entity>& out_entities)
	{
		for (auto iter = in_entities.begin(); iter != in_entities.end(); iter++)
		{
			ECS::Entity entity = iter->first;
			if (const Transform* transform = GetComponent(Transform, entity))
			{
				if (IsInLevel(level, *transform))
				{
					out_entities.push_back(entity);
				}
			}
		}
	}
}