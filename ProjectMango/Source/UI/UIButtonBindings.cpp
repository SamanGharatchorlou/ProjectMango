#include "pch.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Game/States/GameState.h"
#include "UI/UIManager.h"

using namespace ECS;

void SetupButtonActionBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings)
{
	button_bindings[ "RequestCoin" ] =  [](ECS::Entity entity) {
		ActionRequest& action_request = AddComponent(ActionRequest, Faction::GetPlayer());
		action_request.request = ActionRequest::CollectCoin;
		action_request.target = entity; 
	};

	button_bindings[ "RequestCard" ] =  [](ECS::Entity entity) {
		ActionRequest& action_request = AddComponent(ActionRequest, Faction::GetPlayer());
		action_request.request = ActionRequest::AquireCard;
		action_request.target = entity;
	};

	button_bindings[ "EndTurnButton" ] =  [](ECS::Entity entity) {
		if(TurnState* turn_state = GetComponent(TurnState, Faction::GetPlayer()))
		{
			turn_state->tryEndTurn = true;

			Sprite& sprite = GetComponentRef(Sprite, entity);
			sprite.params.disabled = true;
		} 
	};
		
	button_bindings[ "ReturnCoinsButton" ] =  [](ECS::Entity entity) {
		ActionRequest& action_request = AddComponent(ActionRequest, Faction::GetPlayer());
		action_request.request = ActionRequest::ReturnCoins;
		action_request.target = entity; 
	};

	button_bindings[ "AutoConfirmTurn" ] =  [](ECS::Entity entity) {
		Sprite& sprite = GetComponentRef(Sprite, entity);
		UIButton& button = GetComponentRef(UIButton, entity);
		sprite.SetTexture(button.toggle ? "tick" : "cross");

		if(GameState* game_state = GameState::GetActive())
		{
			game_state->autoConfirmTurn = button.toggle;
		}
	};

	button_bindings["ChooseParentRelic"] = [](ECS::Entity entity) {

		Entity player = Faction::GetPlayer();
		Inventory* player_inventory = GetComponent(Inventory, player);
		if (!player_inventory)
			return;

		Entity parent = GetParent(entity);
		if (Inventory* inventory = GetComponent(Inventory, parent))
		{
			for (u32 i = 0; i < inventory->relics.size(); i++)
			{
				player_inventory->relics.push_back(inventory->relics[i]);
			}
		}

		UIManager& ui_manager = UIManager::Get();
		ui_manager.CloseScreen("RelicRewardScreen");


	};
}

void SetupButtonUIBindings(std::unordered_map<BasicString, std::function<void(ECS::Entity)>>& button_bindings)
{
	button_bindings[ "EndTurnButton" ] =  [](ECS::Entity entity) {
		Sprite& sprite = GetComponentRef(Sprite, entity);
		sprite.params.disabled = true;

		if(const GameState* game_state = GameState::GetActive())
		{
			if(game_state->autoConfirmTurn)
				return;
		}

		if(const TurnState* turn_state = GetComponent(TurnState, Faction::GetPlayer()))
		{
			if(turn_state->IsCurrentTurn())
			{
				Sprite& sprite = GetComponentRef(Sprite, entity);
				sprite.params.disabled = turn_state->CanAquireMoreResources();

				if( !sprite.params.disabled )
					int a = 4;

				
				bool my_bool = turn_state->CanAquireMoreResources();
				int a = 4;
			}
		} 
	};

	button_bindings[ "ReturnCoinsButton" ] =  [](ECS::Entity entity) {
		Sprite& sprite = GetComponentRef(Sprite, entity);
		sprite.params.disabled = true;
		if(const TurnState* turn_state = GetComponent(TurnState, Faction::GetPlayer()))
		{
			if(turn_state->IsCurrentTurn())
			{
				bool has_collected_coins = false;
				for( u32 i = 0; i < Colour::Count; i++ )
				{
					if(turn_state->collectedCoins[i] > 0)
					{
						has_collected_coins = true;
						break;
					}
				}

				sprite.params.disabled = !has_collected_coins;
			}
		} 
	};
}