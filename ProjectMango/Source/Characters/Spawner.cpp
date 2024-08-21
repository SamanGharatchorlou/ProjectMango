#include "pch.h"
#include "Spawner.h"

#include "Characters/Enemies/Enemies.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Level.h"
#include "ECS/EntityCoordinator.h"

ECS::Entity EnemySpawn::Spawn(const ECS::TileMap& map)
{
	ECS::Entity entity = Enemy::Create();

	//VectorF spawn_pos = map.tileMap.enemySpawnArea.Center();

	//ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	//ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
	//transform.rect.SetCenter(spawn_pos);
	////transform.targetCenterPosition = spawn_pos;

	return entity;
}


void EnemySpawn::SpawnAll(const ECS::TileMap& map)
{
	Spawn(map);
}