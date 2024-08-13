#include "pch.h"
#include "TileMap.h"

#include "ECS/EntityCoordinator.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "System/Window.h"

VectorF ECS::TileMap::GetMapSizeRatio()
{
	const GameState* gs = GameData::Get().systemStateManager->GetActiveState<GameState>();
	if(!gs)
		return VectorF();
		
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	const ECS::TileMap& map = ecs->GetComponentRef(TileMap, gs->activeMap);

	const VectorF window_size = GameData::Get().window->size();
	return window_size / map.tileMap.mapSize;
}

const ECS::TileMap* ECS::TileMap::GetActive()
{
	if(const GameState* gs = GameData::Get().systemStateManager->GetActiveState<GameState>())
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		return ecs->GetComponent(TileMap, gs->activeMap);
	}
		
	return nullptr;
}