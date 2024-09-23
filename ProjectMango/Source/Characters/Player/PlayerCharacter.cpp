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
	ecs->AddComponent(Transform, s_playerEntity);
	ecs->AddComponent(Physics, s_playerEntity);
	ecs->AddComponent(Animator, s_playerEntity);
	ecs->AddComponent(Sprite, s_playerEntity);
	ecs->AddComponent(Collider, s_playerEntity);
	ecs->AddComponent(PlayerController, s_playerEntity);
	ecs->AddComponent(CharacterState, s_playerEntity);
	ecs->AddComponent(Health, s_playerEntity);

	// Transform
	ECS::Transform& transform = ecs->GetComponentRef(Transform, s_playerEntity);
	transform.Init(config->values, VectorF::zero());
	
	// MovementPhysics
	ECS::Physics& physics = ecs->GetComponentRef(Physics, s_playerEntity);
	physics.Init(config->values);

	// Animation
	ECS::Animator& animator = ecs->GetComponentRef(Animator, s_playerEntity);
	animator.Init(config->animation.c_str());

	// Sprite
	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, s_playerEntity);
	sprite.renderLayer = 5;
	
	// Collider
	ECS::Collider& collider = ecs->GetComponentRef(Collider, s_playerEntity);
	collider.SetBaseRect(RectF(VectorF::zero(), transform.size));
	collider.SetFlag(ECS::Collider::IsPlayer);
	collider.SetFlag(ECS::Collider::CanBump);
	collider.UpdateFromTransform();

	const ECS::Animation& animation = animator.GetActiveAnimation();
	collider.SetRelativeRect(animation.entityColliderPos, animation.entityColliderSize);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->GetComponentRef(CharacterState, s_playerEntity);

	character_state.config = player_config;
	
	// Health
	ECS::Health& health = ecs->GetComponentRef(Health, s_playerEntity);
	health.Init(config->values);

	Camera* camera = Camera::Get();
	camera->targetEntity = s_playerEntity;
	
	DebugMenu::SelectEntity(s_playerEntity);
	return s_playerEntity;
} 