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

	s_playerEntity = Character::CreateBasic(id, player_config, VectorF());
	ecs->AddComponent(PlayerController, s_playerEntity);

	// Collider
	ECS::Collider& collider = ecs->GetComponentRef(Collider, s_playerEntity);
	collider.SetFlag(ECS::Collider::IsPlayer);
	collider.SetFlag(ECS::Collider::CanBump);

	// Sprite
	ECS::Sprite& sprite = ecs->GetComponentRef(Sprite, s_playerEntity);
	sprite.renderLayer = 5;

	// CharacterState
	ECS::CharacterState& character_state = ecs->AddComponent(CharacterState, s_playerEntity);
	character_state.config = player_config;
	character_state.Init(config);

	// Spellbook
	ECS::SpellBook& spell_book = ecs->AddComponent(SpellBook, s_playerEntity);
	spell_book.SetSpellSlot(0, "Lightning");
	
	Camera* camera = Camera::Get();
	camera->targetEntity = s_playerEntity;
	
	if(DebugMenu::GetSelectedEntity() == ECS::EntityInvalid)
		DebugMenu::SelectEntity(s_playerEntity);

	return s_playerEntity;
} 
