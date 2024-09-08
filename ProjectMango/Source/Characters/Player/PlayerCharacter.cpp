#include "pch.h"
#include "PlayerCharacter.h"

#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"

#include "Animations/AnimationReader.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Level.h"
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
	
	const PlayerDataConfig* gs = ConfigManager::Get()->GetConfig<PlayerDataConfig>("PlayerDataConfig");

	ecs->entities.KillEntity(s_playerEntity);
	s_playerEntity = ecs->CreateEntity("Player");

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, s_playerEntity);
	const VectorF size = VectorF(gs->settings.at("size_x"), gs->settings.at("size_y"));
	transform.SetWorldPosition(ECS::Level::GetSpawnPos("PlayerStart") - (size / 2.0f));
	transform.size = size;
	
	// MovementPhysics
	ECS::Physics& physics = ecs->AddComponent(Physics, s_playerEntity);
	physics.applyGravity = true;	
	physics.acceleration = VectorF(gs->settings.at("acceleration_x"), gs->settings.at("acceleration_y"));
	physics.maxSpeed.x = gs->settings.at("max_run_speed");
	physics.maxSpeed.y = gs->settings.at("max_fall_speed");

	// Animation
	ECS::Animator& animation = ecs->AddComponent(Animator, s_playerEntity);
	AnimationReader::BuildAnimatior( "PandaHeroAnimations", animation.animations );
	animation.activeAnimation = 0;

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
	health.maxHealth = gs->settings.at("max_health");
	health.currentHealth = health.maxHealth;

	DebugMenu::SelectEntity(s_playerEntity);

	
	collider.SetFlag(ECS::Collider::TerrainOnly);

	return s_playerEntity;
} 