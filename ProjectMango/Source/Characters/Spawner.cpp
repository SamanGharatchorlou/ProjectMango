#include "pch.h"
#include "Spawner.h"

#include "Characters/Player/PlayerCharacter.h"
#include "Characters/Enemies/Enemies.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Level.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"


#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"

ECS::Entity PlayerSpawn::Spawn()
{
	State& state = GameData::Get().systemStateManager->mStates.Top();
	if( const GameState* gs = dynamic_cast<const GameState*>(&state) )
	{
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		const ECS::Level& level = ecs->GetComponentRef(Level, gs->activeLevel);
		return PlayerSpawn::Spawn(level.playerSpawn);
	}

	return ECS::EntityInvalid;
}

ECS::Entity PlayerSpawn::Spawn(VectorF spawn_pos)
{
	ECS::Entity entity = Player::Create();

	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Transform& transform = ecs->GetComponentRef(Transform, entity);
	transform.position = spawn_pos;
	//transform.rect.SetBotCenter(spawn_pos);
	return entity;
}

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