#pragma once

namespace ECS
{
	struct  TileMap;
}

namespace PlayerSpawn
{
	ECS::Entity Spawn();
	ECS::Entity Spawn(VectorF spawn_pos);
}

namespace EnemySpawn
{
	ECS::Entity Spawn(const ECS::TileMap& map);
	void SpawnAll(const ECS::TileMap& map);
}