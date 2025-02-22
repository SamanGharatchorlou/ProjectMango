#include "pch.h"
#include "Biome.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "Game/Camera/Camera.h"
#include "Core/Helpers.h"

namespace ECS
{
	Biome::Biome()
	{
		aabb[0] = VectorF();
		aabb[1] = VectorF();
	}

	const Entity Biome::GetActive()
	{
		State& state = GameData::Get().systemStateManager->mStates.Top();
		if (const GameState* gs = dynamic_cast<const GameState*>(&state))
		{
			return gs->activeLevel;
		}

		return EntityInvalid;
	}

	const Biome& Biome::GetActiveBiome()
	{	
		return GetComponentRef(Biome, GetActive());
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

	const Level* Biome::GetLevelFromIndex(u32 level_index)
	{
		const std::vector<ECS::Level>& levels = GetActiveBiome().levels;
		for (u32 i = 0; i < levels.size(); i++)
		{
			if (levels[i].index == level_index)
				return &levels[i];
		}

		return nullptr;
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

	
	RectF Level::GetWalkableTileRect(VectorI index) const 
	{
		VectorF tile_size = layers.front().tileSize;
		VectorF tile_pos = index.toFloat() * tile_size + worldPos;

		return RectF(tile_pos, tile_size);
	}


	BasicString EntityMetaData::ConfigId() const
	{
		char buffer[64];
		if(idPostfix.empty())
			snprintf(buffer, 64, "%sConfig", id.c_str());
		else
			snprintf(buffer, 64, "%sConfig", idPostfix.c_str());

		return BasicString(buffer);
	}


	RectF Level::GetBounds() const
	{
		return RectF(worldPos, size);
	}

	bool Level::IsPointInBounds(VectorF world_position) const
	{
		return Contains(GetBounds(), world_position);
	}
}