#include "pch.h"
#include "PlayerCharacter.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"

#include "System/Files/ConfigManager.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

ECS::Entity s_playerEntity = ECS::EntityInvalid;

ECS::Entity Player::Get()
{
	return s_playerEntity;
}

ECS::Entity Player::Spawn()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	
	ecs->entities.KillEntity(s_playerEntity);

	VectorF spawn_pos;
	bool can_spawn = ECS::Biome::GetSpawnPos("PlayerSpawn", spawn_pos);
	if(!can_spawn)
		return ECS::EntityInvalid;

	s_playerEntity = ecs->CreateEntity("Player");

	const PlayerConfig* config = ConfigManager::Get()->GetConfig<PlayerConfig>("PlayerDataConfig");

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, s_playerEntity);
	transform.Init(config->values);
	transform.SetWorldPosition(spawn_pos - (transform.size / 2.0f));
	
	// MovementPhysics
	ECS::Physics& physics = ecs->AddComponent(Physics, s_playerEntity);
	physics.Init(config->values);

	// Animation
	ECS::Animator& animation = ecs->AddComponent(Animator, s_playerEntity);
	animation.Init(config->animation.c_str());

	// Sprite
	ECS::Sprite& sprite = ecs->AddComponent(Sprite, s_playerEntity);
	sprite.renderLayer = 9;
	
	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, s_playerEntity);
	collider.SetBaseRect(RectF(VectorF::zero(), transform.size));
	collider.SetFlag(ECS::Collider::IsPlayer);
	collider.SetFlag(ECS::Collider::CanBump);
	
	// PlayerController
	ECS::PlayerController& player_controller = ecs->AddComponent(PlayerController, s_playerEntity);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->AddComponent(CharacterState, s_playerEntity);
	
	// Health
	ECS::Health& health = ecs->AddComponent(Health, s_playerEntity);
	health.Init(config->values);

	DebugMenu::SelectEntity(s_playerEntity);

	return s_playerEntity;
} 