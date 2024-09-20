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


	const Biome* Biome::GetActiveBiome()
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		return ecs->GetComponent(Biome, GetActive());
	}

	
	const Level& Biome::GetLevel(ECS::Entity entity) const
	{
		VectorF pos = GetPosition(entity);
		return GetLevel(pos);
	}

	const Level& Biome::GetLevel(VectorF position) const
	{
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

	const Level& Biome::GetVisibleLevel() const
	{
		const Camera* camera = Camera::Get();
		return GetLevel(camera->GetRect().Center());
	}

	bool Biome::GetLevelSpawnPos(const char* spawn_id, VectorF& out_pos)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		ECS::Entity level_entity = ECS::Biome::GetActive();
		if (ECS::Biome* biome = ecs->GetComponent(Biome, level_entity))
		{
			const ECS::Level& level = biome->GetVisibleLevel();
			if (level.entities.contains(spawn_id))
			{
				out_pos = level.entities.at(spawn_id);
				return true;
			}
		}

		return false;
	}

	bool Biome::GetBiomeSpawnPos(const char* spawn_id, VectorF& out_pos)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		ECS::Entity level_entity = ECS::Biome::GetActive();
		if (ECS::Biome* biome = ecs->GetComponent(Biome, level_entity))
		{
			for (u32 i = 0; i < biome->levels.size(); i++)
			{
				const ECS::Level& level = biome->levels[i];
				if (level.entities.contains(spawn_id))
				{
					out_pos = level.entities.at(spawn_id);
					return true;
				}
			}
		}

		return false;
	}
}