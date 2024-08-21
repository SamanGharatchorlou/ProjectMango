#pragma once

namespace ECS
{
	struct  TileMap;
}

namespace EnemySpawn
{
	ECS::Entity Spawn(const ECS::TileMap& map);
	void SpawnAll(const ECS::TileMap& map);
}