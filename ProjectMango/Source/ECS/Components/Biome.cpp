#include "pch.h"
#include "Biome.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "Game/Camera/Camera.h"


namespace ECS
{
	const Entity Biome::GetActive()
	{
		State& state = GameData::Get().systemStateManager->mStates.Top();
		if (const GameState* gs = dynamic_cast<const GameState*>(&state))
		{
			EntityCoordinator* ecs = GameData::Get().ecs;
			return gs->activeLevel;
		}

		return EntityInvalid;
	}


	const Biome& Biome::GetActiveBiome()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		return ecs->GetComponentRef(Biome, GetActive());
	}

	
	const Level& Biome::GetLevel(ECS::Entity entity)
	{
		VectorF pos = GetPosition(entity);
		return GetLevel(pos);
	}

	const Level& Biome::GetLevel(VectorF position)
	{
		const std::vector<ECS::Level>& levels = GetActiveBiome().levels;
		for( u32 i = 0; i < levels.size(); i++ )
		{
			const VectorF world_pos = levels[i].worldPos;
			if(position.x > world_pos.x && position.y > world_pos.y)
			{
				const VectorF world_pos_end = world_pos + levels[i].size;
				if(position.x < world_pos_end.x && position.y < world_pos_end.y)
				{
					return levels[i];
				}
			}
		}

		ASSERT(levels.size() > 0, "We have no levels yet, biome has not been parsed");
		return levels.front();
	}

	const Level& Biome::GetVisibleLevel()
	{
		const Camera* camera = Camera::Get();
		return GetLevel(camera->GetRect().Center());
	}

	
	VectorI Level::GetTileIndex(VectorF position) const
	{
		VectorF local_pos = position - worldPos;
		if(local_pos.isPositive())
		{
			if(local_pos.x <= size.x && local_pos.y <= size.y)
			{
				VectorF tile_size = layers.front().tileSize;

				return (local_pos / tile_size).toInt();
			}
		}

		return VectorI(-1,-1);
	}
}