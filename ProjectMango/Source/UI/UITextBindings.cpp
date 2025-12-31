#include "pch.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Entities/CardRegistry.h"
#include "ECS/Components/AIComponents.h"
#include "Entities/ResourceBank.h"

using namespace ECS;

static void InventoryCoinsBindings(std::unordered_map<BasicString, std::function<BasicString(Entity entity)>>& text_bindings)
{
	text_bindings[ "PlayerInventoryCoins" ] =  [](Entity entity) {
		if(const Inventory* inventory = GetComponent(Inventory, Faction::GetPlayer()))
		{
			Colour::Type colour = GetComponentRef(Colour, entity).colour;
			return BasicString(inventory->coins[colour]);
		}
		return BasicString(""); 
	};
	text_bindings[ "AIInventoryCoins" ] =  [](Entity entity) {
		if(const Inventory* inventory = GetComponent(Inventory, Faction::GetEnemy()))
		{
			Colour::Type colour = GetComponentRef(Colour, entity).colour;
			return BasicString(inventory->coins[colour]);
		}
		return BasicString(""); 
	};
}

static void TurnCoinsBindings(std::unordered_map<BasicString, std::function<BasicString(Entity)>>& text_bindings)
{
	text_bindings[ "PlayerTurnCoins" ] =  [](Entity entity) {
		if(const TurnState* turn_state = GetComponent(TurnState, Faction::GetPlayer()))
		{
			Colour::Type colour = GetComponentRef(Colour, entity).colour;
			return BasicString(turn_state->collectedCoins[colour]);
		}

		return BasicString(""); };

	text_bindings[ "AITurnCoins" ] =  [](Entity entity) {
		if(const TurnState* turn_state = GetComponent(TurnState, Faction::GetEnemy()))
		{
			Colour::Type colour = GetComponentRef(Colour, entity).colour;
			return BasicString(turn_state->collectedCoins[colour]);
		}

		return BasicString(""); 
	};
}

static void CoinStackSupplyBindings(std::unordered_map<BasicString, std::function<BasicString(Entity)>>& text_bindings)
{
	text_bindings[ "CoinStackSupply_White" ] =  [](Entity entity) {
		CoinStack& cs = GetCoinStack(Faction::None, (u32)Colour::White);
		return BasicString(cs.remaining); 
	};

	text_bindings[ "CoinStackSupply_Blue" ] =  [](Entity entity) {
		CoinStack& cs = GetCoinStack(Faction::None, (u32)Colour::Blue);
		return BasicString(cs.remaining);
	};

	text_bindings[ "CoinStackSupply_Black" ] =  [](Entity entity) {
		CoinStack& cs = GetCoinStack(Faction::None, (u32)Colour::Black);
		return BasicString(cs.remaining); 
	 };

	text_bindings[ "CoinStackSupply_Red" ] =  [](Entity entity) {
		CoinStack& cs = GetCoinStack(Faction::None, (u32)Colour::Red);
		return BasicString(cs.remaining); 
	 };

	text_bindings[ "CoinStackSupply_Green" ] =  [](Entity entity) {
		CoinStack& cs = GetCoinStack(Faction::None, (u32)Colour::Green);
		return BasicString(cs.remaining); 
	};
}

static void CardPowerBindings(std::unordered_map<BasicString, std::function<BasicString(Entity)>>& text_bindings)
{
	text_bindings[ "PlayerCardPower" ] =  [](Entity entity) {
		if(const Inventory* inventory = GetComponent(Inventory, Faction::GetPlayer()))
		{
			int card_power[Colour::Count];
			inventory->GetCardPower(card_power, Colour::Count);

			Colour::Type colour = GetComponentRef(Colour, entity).colour;
			return BasicString(card_power[colour]);
		}
		return BasicString(""); 
	};
	text_bindings[ "AICardPower" ] =  [](Entity entity) {
		if(const Inventory* inventory = GetComponent(Inventory, Faction::GetEnemy()))
		{
			int card_power[Colour::Count];
			inventory->GetCardPower(card_power, Colour::Count);

			Colour::Type colour = GetComponentRef(Colour, entity).colour;
			return BasicString(card_power[colour]);
		}
		return BasicString(""); 
	};
}

void SetupTextUIBindings(std::unordered_map<BasicString, std::function<BasicString(Entity)>>& text_bindings)
{
	InventoryCoinsBindings(text_bindings);
	TurnCoinsBindings(text_bindings);
	CardPowerBindings(text_bindings);

	text_bindings[ "TurnNumber" ] =  [](Entity entity) {
		const State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(const GameState* game_state = dynamic_cast<const GameState*>(&state))
		{
			char buffer[32];
			snprintf(buffer, 32, "Turn: %d", game_state->turnIndex + 1);
			return BasicString(buffer);
		}
					
		return BasicString(""); 
	};

	text_bindings[ "GameOverResult" ] =  [](Entity entity) {
		const State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(const GameState* game_state = dynamic_cast<const GameState*>(&state))
		{
			if(game_state->gameOver)
			{
				if(const ECS::Health* health = GetComponent(Health, Faction::GetEnemy()))
				{
					if(health->currentHealth <= 0)
					{
						UIText& text = GetComponentRef(UIText, entity);
						text.SetColour(SColour::Green);

						return BasicString("VICTORY");
					}
				}
				
				if(const ECS::Health* health = GetComponent(Health, Faction::GetPlayer()))
				{
					if(health->currentHealth <= 0)
					{
						UIText& text = GetComponentRef(UIText, entity);
						text.SetColour(SColour::Red);

						return BasicString("DEFEATED");
					}
				}
			}
		}
					
		return BasicString(""); 
	};

	text_bindings[ "PlayerPoints" ] =  [](Entity entity) {
		if(const Inventory* inventory = GetComponent(Inventory, Faction::GetPlayer()))
		{
			int total_points = 0;
			for( int card_index : inventory->cards )
			{
				const Card* card = CardRegistry::LookupCard(card_index);
				total_points += card->points;
			}

			char buffer[32];
			snprintf(buffer, 32, "Points: %d", total_points);
			return BasicString(buffer);
		}
					
		return BasicString(""); 
	};

	text_bindings[ "AIPoints" ] =  [](Entity entity) {
		if(const Inventory* inventory = GetComponent(Inventory, Faction::GetEnemy()))
		{
			int total_points = 0;
			for( int card_index : inventory->cards )
			{
				const Card* card = CardRegistry::LookupCard(card_index);
				total_points += card->points;
			}

			char buffer[32];
			snprintf(buffer, 32, "Points: %d", total_points);
			return BasicString(buffer);
		}
					
		return BasicString(""); 
	};

}