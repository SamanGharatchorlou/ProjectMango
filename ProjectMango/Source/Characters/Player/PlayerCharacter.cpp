#include "pch.h"
#include "PlayerCharacter.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"
#include "Game/Camera/Camera.h"

#include "System/Files/ConfigManager.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"

ECS::Entity s_playerEntity = ECS::EntityInvalid;

ECS::Entity Player::Get()
{
	return s_playerEntity;
}

ECS::Entity Player::Spawn(const char* id, const char* player_config)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ecs->entities.KillEntity(s_playerEntity);

	const ObjectConfig* config = ConfigManager::Get()->GetConfig<ObjectConfig>(player_config);

	s_playerEntity = ecs->CreateEntity(id);

	// Transform
	ECS::Transform& transform = ecs->AddComponent(Transform, s_playerEntity);
	transform.Init(config->values);
	
	// MovementPhysics
	ECS::Physics& physics = ecs->AddComponent(Physics, s_playerEntity);
	physics.Init(config->values);

	// Animation
	ECS::Animator& animator = ecs->AddComponent(Animator, s_playerEntity);
	animator.Init(config->animation.c_str());

	// Sprite
	ECS::Sprite& sprite = ecs->AddComponent(Sprite, s_playerEntity);
	sprite.renderLayer = 5;
	
	// Collider
	ECS::Collider& collider = ecs->AddComponent(Collider, s_playerEntity);
	collider.SetBaseRect(RectF(VectorF::zero(), transform.size));
	collider.SetFlag(ECS::Collider::IsPlayer);
	collider.SetFlag(ECS::Collider::CanBump);
	
	const ECS::Animation& animation = animator.GetActiveAnimation();
	collider.SetRelativeRect(animation.entityColliderPos, animation.entityColliderSize);

	// PlayerController
	ECS::PlayerController& player_controller = ecs->AddComponent(PlayerController, s_playerEntity);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->AddComponent(CharacterState, s_playerEntity);
	
	// Health
	ECS::Health& health = ecs->AddComponent(Health, s_playerEntity);
	health.Init(config->values);

	// set the position after all the bits have been setup
	transform.SetWorldPosition(VectorF::zero());

	Camera* camera = Camera::Get();
	camera->targetEntity = s_playerEntity;
	
	DebugMenu::SelectEntity(s_playerEntity);
	return s_playerEntity;
} 