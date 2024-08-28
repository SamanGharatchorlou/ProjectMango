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
	s_playerEntity = ecs->CreateEntity("Player");

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, s_playerEntity);
	VectorF size = VectorF(412, 122);
	transform.SetWorldPosition(ECS::Level::GetSpawnPos("PlayerStart") - (size / 2.0f));
	transform.size = size;
	
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
	collider.SetBaseRect(RectF(VectorF::zero(), transform.size));
	
	// PlayerController
	ECS::PlayerController& player_controller = ecs->AddComponent(PlayerController, s_playerEntity);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->AddComponent(CharacterState, s_playerEntity);
	
	// Health
	ECS::Health& health = ecs->AddComponent(Health, s_playerEntity);
	health.maxHealth = 100;
	health.currentHealth = 100;

	DebugMenu::SelectEntity(s_playerEntity);

	return s_playerEntity;
}