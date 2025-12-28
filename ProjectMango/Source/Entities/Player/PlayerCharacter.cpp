#include "pch.h"
#include "PlayerCharacter.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Game/Camera/Camera.h"
#include "Entities/EntityBuilder.h"

#include "Debugging/ImGui/ImGuiMainWindows.h"

using namespace ECS;

Entity Player::Spawn(const EntityMetaData& emd)
{
	ecs->entities.KillEntity(Target::GetPlayer());

	Entity player = CreateEntity( "player" );
	
	AddComponent(Transform, player);
	AddComponent(PlayerController, player);

	Inventory& inventory = AddComponent(Inventory, player);

	// debug
	//for( u32 i = 0; i < Colour::Count; i++ )
	//{
	//	inventory.coins[i] = 2;
	//}

	TurnState& turn = AddComponent(TurnState, player);

	// Health
	Health& health = AddComponent(Health, player);
	health.currentHealth = 10;
	health.maxHealth = 10;
	
	AddComponent(EntityState, player);

	if(DebugMenu::GetSelectedEntity() == EntityInvalid)
		DebugMenu::SelectEntity(player);

	return player;
} 
