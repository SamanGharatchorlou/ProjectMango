#include "pch.h"
#include "PlayerCharacter.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Collider.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GunComponents.h"
#include "ECS/Components/Biome.h"
#include "Game/Camera/Camera.h"
#include "Scene/SceneParsing/EntityBuilder.h"
#include "Entities/Weapons/GunEntityBuilder.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"

using namespace ECS;

Entity s_playerEntity = EntityInvalid;

Entity Player::Get()
{
	return s_playerEntity;
}

Entity Player::Spawn(const EntityMetaData& emd)
{
	ecs->entities.KillEntity(s_playerEntity);
	s_playerEntity = Character::CreateBasic(emd);
	const Config* config = GetConfig(s_playerEntity);

	// PlayerController
	AddComponent(PlayerController, s_playerEntity);

	// Collider
	Collider& collider = GetComponentRef(Collider, s_playerEntity);
	collider.SetFlag(Collider::IsPlayer);
	collider.SetFlag(Collider::CanBump);

	// Sprite
	Sprite& sprite = GetComponentRef(Sprite, s_playerEntity);
	sprite.renderLayer = RenderLayer::Characters;

	// CharacterState
	CharacterState& character_state = AddComponent(CharacterState, s_playerEntity);
	character_state.Init(config);

	// Spellbook
	//SpellBook& spell_book = AddComponent(SpellBook, s_playerEntity);
	//spell_book.SetSpellSlot(0, "Fireball");

	// Firearm
	EquipFirearm(s_playerEntity, "BasicRifle");
	
	Camera* camera = Camera::Get();
	camera->targetEntity = s_playerEntity;
	
	if(DebugMenu::GetSelectedEntity() == EntityInvalid)
		DebugMenu::SelectEntity(s_playerEntity);

	return s_playerEntity;
} 
