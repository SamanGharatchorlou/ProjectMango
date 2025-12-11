#include "pch.h"
#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Player/PlayerCharacter.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"

using namespace ECS;

static void InventoryCoinsBindings(std::unordered_map<BasicString, std::function<BasicString()>>& text_bindings)
{
	text_bindings[ "InventoryCoins_White" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
			return BasicString(inventory->coins[Coin::White]);
		return BasicString(""); };

	text_bindings[ "InventoryCoins_Blue" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
			return BasicString(inventory->coins[Coin::Blue]);
		return BasicString(""); };

	text_bindings[ "InventoryCoins_Black" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
			return BasicString(inventory->coins[Coin::Black]);
		return BasicString(""); };

	text_bindings[ "InventoryCoins_Red" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
			return BasicString(inventory->coins[Coin::Red]);	
		return BasicString(""); };

	text_bindings[ "InventoryCoins_Green" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
			return BasicString(inventory->coins[Coin::Green]);	
		return BasicString(""); };
}

static void TurnCoinsBindings(std::unordered_map<BasicString, std::function<BasicString()>>& text_bindings)
{
	text_bindings[ "TurnCoins_White" ] =  []() {
		if(TurnState* turn_state = GetComponent(TurnState, Player::Get()))
			return BasicString(turn_state->collectedCoins[Coin::White]);
		return BasicString(""); };

	text_bindings[ "TurnCoins_Blue" ] =  []() {
		if(TurnState* turn_state = GetComponent(TurnState, Player::Get()))
			return BasicString(turn_state->collectedCoins[Coin::Blue]);
		return BasicString(""); };

	text_bindings[ "TurnCoins_Black" ] =  []() {
		if(TurnState* turn_state = GetComponent(TurnState, Player::Get()))
			return BasicString(turn_state->collectedCoins[Coin::Black]);
		return BasicString(""); };

	text_bindings[ "TurnCoins_Red" ] =  []() {
		if(TurnState* turn_state = GetComponent(TurnState, Player::Get()))
			return BasicString(turn_state->collectedCoins[Coin::Red]);	
		return BasicString(""); };

	text_bindings[ "TurnCoins_Green" ] =  []() {
		if(TurnState* turn_state = GetComponent(TurnState, Player::Get()))
			return BasicString(turn_state->collectedCoins[Coin::Green]);	
		return BasicString(""); };
}

static void CoinStackSupplyBindings(std::unordered_map<BasicString, std::function<BasicString()>>& text_bindings)
{
	text_bindings[ "CoinStackSupply_White" ] =  []() {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Coin::White))
			return BasicString(cs->remaining);
		return BasicString(""); };

	text_bindings[ "CoinStackSupply_Blue" ] =  []() {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Coin::Blue))
			return BasicString(cs->remaining);
		return BasicString(""); };

	text_bindings[ "CoinStackSupply_Black" ] =  []() {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Coin::Black))
			return BasicString(cs->remaining);
		return BasicString(""); };

	text_bindings[ "CoinStackSupply_Red" ] =  []() {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Coin::Red))
			return BasicString(cs->remaining);
		return BasicString(""); };

	text_bindings[ "CoinStackSupply_Green" ] =  []() {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Coin::Green))
			return BasicString(cs->remaining);	
		return BasicString(""); };
}

static void CardPowerBindings(std::unordered_map<BasicString, std::function<BasicString()>>& text_bindings)
{
	text_bindings[ "CardPower_White" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
		{
			int card_power[Coin::Count];
			inventory->GetCardPower(card_power, Coin::Count);
			return BasicString(card_power[Coin::White]);
		}
		return BasicString(""); };

	text_bindings[ "CardPower_Blue" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
		{
			int card_power[Coin::Count];
			inventory->GetCardPower(card_power, Coin::Count);
			return BasicString(card_power[Coin::Blue]);
		}
		return BasicString(""); };

	text_bindings[ "CardPower_Black" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
		{
			int card_power[Coin::Count];
			inventory->GetCardPower(card_power, Coin::Count);
			return BasicString(card_power[Coin::Black]);
		}
		return BasicString(""); };

	text_bindings[ "CardPower_Red" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
		{
			int card_power[Coin::Count];
			inventory->GetCardPower(card_power, Coin::Count);
			return BasicString(card_power[Coin::Red]);
		}	
		return BasicString(""); };

	text_bindings[ "CardPower_Green" ] =  []() {
		if(Inventory* inventory = GetComponent(Inventory, Player::Get()))
		{
			int card_power[Coin::Count];
			inventory->GetCardPower(card_power, Coin::Count);
			return BasicString(card_power[Coin::Green]);
		}
		return BasicString(""); };
}

void SetupTextBindings(std::unordered_map<BasicString, std::function<BasicString()>>& text_bindings)
{
	InventoryCoinsBindings(text_bindings);
	TurnCoinsBindings(text_bindings);
	CoinStackSupplyBindings(text_bindings);
	CardPowerBindings(text_bindings);

	text_bindings[ "TurnNumber" ] =  []()
			{
				State& state = GameData::Get().systemStateManager->mStates.getActiveState();
				if(GameState* game_state = dynamic_cast<GameState*>(&state))
				{
					return BasicString(game_state->turnIndex + 1);
				}
					
				return BasicString("");
			};
}