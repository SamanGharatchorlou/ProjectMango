#include "pch.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/UIComponents.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Entities/CardRegistry.h"
#include "ECS/Components/AIComponents.h"

using namespace ECS;

static void InventoryCoinsBindings(std::unordered_map<BasicString, std::function<BasicString(Entity entity)>>& text_bindings)
{
	text_bindings[ "PlayerInventoryCoins" ] =  [](Entity entity) {
		if(Inventory* inventory = GetComponent(Inventory, Target::GetPlayer()))
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
		if(TurnState* turn_state = GetComponent(TurnState, Target::GetPlayer()))
		{
			Colour::Type colour = GetComponentRef(Colour, entity).colour;
			return BasicString(turn_state->collectedCoins[colour]);
		}

		return BasicString(""); };

	text_bindings[ "AITurnCoins" ] =  [](Entity entity) {
		Entity ai = Target::GetEnemy();
		if(ai != EntityInvalid)
		{
			if(TurnState* turn_state = GetComponent(TurnState, ai))
			{
				Colour::Type colour = GetComponentRef(Colour, entity).colour;
				return BasicString(turn_state->collectedCoins[colour]);
			}
		}

		return BasicString(""); 
	};
}

static void CoinStackSupplyBindings(std::unordered_map<BasicString, std::function<BasicString(Entity)>>& text_bindings)
{
	text_bindings[ "CoinStackSupply_White" ] =  [](Entity entity) {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Colour::White))
			return BasicString(cs->remaining);
		return BasicString(""); };

	text_bindings[ "CoinStackSupply_Blue" ] =  [](Entity entity) {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Colour::Blue))
			return BasicString(cs->remaining);
		return BasicString(""); };

	text_bindings[ "CoinStackSupply_Black" ] =  [](Entity entity) {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Colour::Black))
			return BasicString(cs->remaining);
		return BasicString(""); };

	text_bindings[ "CoinStackSupply_Red" ] =  [](Entity entity) {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Colour::Red))
			return BasicString(cs->remaining);
		return BasicString(""); };

	text_bindings[ "CoinStackSupply_Green" ] =  [](Entity entity) {
		if(CoinStack* cs = CoinStack::GetCoinStack(ECS::Colour::Green))
			return BasicString(cs->remaining);	
		return BasicString(""); };
}

static void CardPowerBindings(std::unordered_map<BasicString, std::function<BasicString(Entity)>>& text_bindings)
{
	text_bindings[ "PlayerCardPower" ] =  [](Entity entity) {
		if(Inventory* inventory = GetComponent(Inventory, Target::GetPlayer()))
		{
			int card_power[Colour::Count];
			inventory->GetCardPower(card_power, Colour::Count);

			Colour::Type colour = GetComponentRef(Colour, entity).colour;
			return BasicString(card_power[colour]);
		}
		return BasicString(""); 
	};
	text_bindings[ "AICardPower" ] =  [](Entity entity) {
		
		Entity ai = Target::GetEnemy();
		if(Inventory* inventory = GetComponent(Inventory, ai))
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
	//CoinStackSupplyBindings(text_bindings);
	CardPowerBindings(text_bindings);

	text_bindings[ "TurnNumber" ] =  [](Entity entity) {
		State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(GameState* game_state = dynamic_cast<GameState*>(&state))
		{
			char buffer[32];
			snprintf(buffer, 32, "Turn: %d", game_state->turnIndex + 1);
			return BasicString(buffer);
		}
					
		return BasicString(""); 
	};

	text_bindings[ "GameOverResult" ] =  [](Entity entity) {
		State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(GameState* game_state = dynamic_cast<GameState*>(&state))
		{
			if(game_state->gameOver)
			{
				Entity ai = Target::GetEnemy();
				if(ECS::Health* health = GetComponent(Health, ai))
				{
					if(health->currentHealth <= 0)
					{
						UIText& text = GetComponentRef(UIText, entity);
						text.SetColour(SColour::Green);

						return BasicString("VICTORY");
					}
				}
				
				if(ECS::Health* health = GetComponent(Health, Target::GetPlayer()))
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
		if(Inventory* inventory = GetComponent(Inventory, Target::GetPlayer()))
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
		
		Entity ai = Target::GetEnemy();
		if(Inventory* inventory = GetComponent(Inventory, ai))
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