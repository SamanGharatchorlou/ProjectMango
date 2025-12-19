#include "pch.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Player/PlayerCharacter.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Entities/CardRegistry.h"

using namespace ECS;

void SetupSpriteUIBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings)
{
	//button_bindings[ "InventoryCoins_White" ] =  [](ECS::Entity entity) {
	//	if(Inventory* inventory = GetComponent(Inventory, Target::GetPlayer()))
	//	{
	//		Sprite& sprite = GetComponentRef(Sprite, entity);
	//				
	//		char buffer[32];
	//		snprintf( buffer, 32, "%s%d", "InventoryCoin", inventory->coins[Colour::White]);
	//		sprite.SetTexture(buffer);
	//		sprite.colourMod = SColour(Colour::White);
	//	}
	//};
}