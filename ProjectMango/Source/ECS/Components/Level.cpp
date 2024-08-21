#include "pch.h"
#include "Level.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"


namespace ECS
{
	const ECS::Entity Level::GetActive()
	{
		State& state = GameData::Get().systemStateManager->mStates.Top();
		if (const GameState* gs = dynamic_cast<const GameState*>(&state))
		{
			ECS::EntityCoordinator* ecs = GameData::Get().ecs;
			return gs->activeLevel;
		}

		return ECS::EntityInvalid;
	}
}