#include "pch.h"
#include "PlayerCharacter.h"

#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"

#include "Animations/AnimationReader.h"
#include "Characters/States/PlayerStates.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Level.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"
#include "Graphics/TextureManager.h"
#include "System/Files/ConfigManager.h"

ECS::Entity s_playerEntity = ECS::EntityInvalid;

ECS::Entity Player::Get()
{
	return s_playerEntity;
}

ECS::Entity Player::Spawn()
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;

	ecs->entities.KillEntity(s_playerEntity);
	s_playerEntity = ecs->CreateEntity("Player");

	VectorF spawn_pos;
	ECS::Entity level_entity = ECS::Level::GetActive();
	if (ECS::Level* level = ecs->GetComponent(Level, level_entity))
		spawn_pos = level->playerSpawn;

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, s_playerEntity);
	transform.SetPosition(spawn_pos);
	transform.size = VectorF(412, 122);
	
	// MovementPhysics
	ECS::Physics& physics = ecs->AddComponent(Physics, s_playerEntity);
	physics.applyGravity = true;	
	physics.acceleration = VectorF(100.0f, 100.0f);
	physics.maxSpeed = VectorF(5.0f, 20.0f);

	// Animation
	ECS::Animator& animation = ecs->AddComponent(Animator, s_playerEntity);
	AnimationReader::BuildAnimatior( "PandaHeroAnimations", animation.animations );
	animation.activeAnimation = 0;

	// Sprite
	ECS::Sprite& sprite = ecs->AddComponent(Sprite, s_playerEntity);
	sprite.renderLayer = 9;
	
	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, s_playerEntity);
	collider.SetRect(RectF(VectorF::zero(), transform.size));
	
	// PlayerController
	ECS::PlayerController& player_controller = ecs->AddComponent(PlayerController, s_playerEntity);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->AddComponent(CharacterState, s_playerEntity);
	
	// Health
	ECS::Health& health = ecs->AddComponent(Health, s_playerEntity);
	health.maxHealth = 100;
	health.currentHealth = 100;

	return s_playerEntity;
}