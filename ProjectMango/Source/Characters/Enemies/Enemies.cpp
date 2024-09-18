#include "pch.h"
#include "Enemies.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/AIController.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Physics.h"

#include "System/Files/ConfigManager.h"

ECS::Entity Enemy::Create()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	VectorF spawn_pos;
	bool can_spawn = ECS::Biome::GetSpawnPos("TrainingDummy", spawn_pos);
	if(!can_spawn)
		return ECS::EntityInvalid;

	ECS::Entity entity = ecs->CreateEntity("Enemy");
	const EnemyConfig* config = ConfigManager::Get()->GetConfig<EnemyConfig>("ShockEnemyConfig");

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, entity);
	transform.Init(config->values);
	transform.SetWorldPosition(spawn_pos);

	// MovementPhysics
	ECS::Physics& physics = ecs->AddComponent(Physics, entity);
	physics.Init(config->values);

	ECS::Animator& animation = ecs->AddComponent(Animator, entity);
	animation.Init(config->animation.c_str());

	// Sprite
	ECS::Sprite& sprite = ecs->AddComponent(Sprite, entity);
	sprite.renderLayer = 8;
	
	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, entity);
	collider.SetBaseRect(RectF(VectorF::zero(), transform.size));
	collider.SetFlag(ECS::Collider::IsEnemy);

	// Health
	ECS::Health& health = ecs->AddComponent(Health, entity);
	health.Init(config->values);
	health.invulnerable = true;

	// AI
	ECS::AIController& ai = ecs->AddComponent(AIController, entity);
	
	// Pathing
	ECS::Pathing& pathing = ecs->AddComponent(Pathing, entity);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->AddComponent(CharacterState, entity);

	return entity;
}
