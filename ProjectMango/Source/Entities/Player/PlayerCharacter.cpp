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

ECS::Entity Player::Spawn(const ECS::EntityMetaData& emd)
{
	ecs->entities.KillEntity(s_playerEntity);
	s_playerEntity = Character::CreateBasic(emd);
	const ObjectConfig* config = ECS::GetObjectConfig(s_playerEntity);

	// PlayerController
	AddComponent(PlayerController, s_playerEntity);

	// Collider
	ECS::Collider& collider = GetComponentRef(Collider, s_playerEntity);
	collider.SetFlag(ECS::Collider::IsPlayer);
	//collider.SetFlag(ECS::Collider::CanBump);

	// Sprite
	ECS::Sprite& sprite = GetComponentRef(Sprite, s_playerEntity);
	sprite.renderLayer = 5;

	// CharacterState
	ECS::CharacterState& character_state = AddComponent(CharacterState, s_playerEntity);
	character_state.Init(config);

	// Spellbook
	ECS::SpellBook& spell_book = AddComponent(SpellBook, s_playerEntity);
	spell_book.SetSpellSlot(0, "Lightning");
	
	Camera* camera = Camera::Get();
	camera->targetEntity = s_playerEntity;
	
	if(DebugMenu::GetSelectedEntity() == ECS::EntityInvalid)
		DebugMenu::SelectEntity(s_playerEntity);

	return s_playerEntity;
} 
