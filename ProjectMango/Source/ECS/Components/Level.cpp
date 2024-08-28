#include "pch.h"
#include "Level.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "ECS/EntityCoordinator.h"


namespace ECS
{
	const Entity Level::GetActive()
	{
		State& state = GameData::Get().systemStateManager->mStates.Top();
		if (const GameState* gs = dynamic_cast<const GameState*>(&state))
		{
			EntityCoordinator* ecs = GameData::Get().ecs;
			return gs->activeLevel;
		}

		return EntityInvalid;
	}

	VectorF Level::GetSpawnPos(const char* spawn_id)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		VectorF spawn_pos;
		ECS::Entity level_entity = ECS::Level::GetActive();
		if (ECS::Level* level = ecs->GetComponent(Level, level_entity))
		{
			if (level->spawnPositions.contains(spawn_id))
				spawn_pos = level->spawnPositions[spawn_id];
		}

		return spawn_pos;
	}
}