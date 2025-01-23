#include "pch.h"
#include "PlayerCharacter.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/SpellComponents.h"
#include "ECS/Components/Animator.h"
#include "ECS/Components/Biome.h"
#include "ECS/Components/Physics.h"
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
	ecs->AddComponent(SpellBook, s_playerEntity);

	// Transform
	ECS::Transform& transform = ecs->GetComponentRef(Transform, s_playerEntity);
	ECS::Collider& collider = ecs->GetComponentRef(Collider, s_playerEntity);
	transform.Init(config->values, VectorF::zero(), collider);

	// Collider
	collider.SetFlag(ECS::Collider::IsPlayer);
	collider.SetFlag(ECS::Collider::CanBump);
	
	// MovementPhysics
	ECS::Physics& physics = ecs->GetComponentRef(Physics, s_playerEntity);
	physics.Init(config->values);

	// Animation
	ECS::Animator& animator = ecs->GetComponentRef(Animator, s_playerEntity);
	animator.Init(config);

	// Sprite
	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, s_playerEntity);
	sprite.renderLayer = 5;

	const ECS::Animation& animation = animator.GetActiveAnimation();
	collider.SetRelativeRect(animation.entityColliderPos, animation.entityColliderSize);
	
	// CharacterState
	ECS::CharacterState& character_state = ecs->GetComponentRef(CharacterState, s_playerEntity);
	character_state.config = player_config;
	character_state.Init(config->values);
	
	// Health
	ECS::Health& health = ecs->GetComponentRef(Health, s_playerEntity);
	health.Init(config->values);

	// Spellbook
	ECS::SpellBook& spell_book = ecs->GetComponentRef(SpellBook, s_playerEntity);
	spell_book.SetSpellSlot(0, "Fireball");
	
	//ECS::Rune* rune = new ECS::RuneRebound;
	//spell_book.SetSpellSlotRune(0, rune);

	Camera* camera = Camera::Get();
	camera->targetEntity = s_playerEntity;
	
	if(DebugMenu::GetSelectedEntity() == ECS::EntityInvalid)
		DebugMenu::SelectEntity(s_playerEntity);

	return s_playerEntity;
} 