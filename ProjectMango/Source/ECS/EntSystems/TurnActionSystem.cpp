#include "pch.h"
#include "TurnActionSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Core/Helpers.h"
#include "Entities/CardRegistry.h"

namespace ECS
{
	bool CanAquireMoreResources(TurnState& turn)
	{
		// already collect 2 of the same coins
		for( u32 i = 0; i < Coin::Count; i++ )
		{
			if(turn.collectedCoins[i] >= 2)
				return false;
		}

		// already collected 3 different coins
		int coins_collected = 0;
		for( u32 i = 0; i < Coin::Count; i++ )
		{
			coins_collected += turn.collectedCoins[i];
		}
		if(coins_collected >= 3)
			return false;

		// already collect a card
		if(turn.collectedCard != EntityInvalid)
			return false;

		return true;
	}

	bool CanAffordCard(TurnState& turn, Card& card)
	{
		if(Inventory* inventory = GetComponent(Inventory, turn.entity))
		{
			int buying_power[Coin::Count];
			inventory->GetBuyingPower(buying_power, Coin::Count);
			for( u32 i = 0; i < Coin::Count; i++ )
			{
				if(buying_power[i] < card.cost[i])
					return false;
			}

			return true;
		}

		return false;
	}

	static bool ExecuteAction(ActionRequest& action_request, TurnState& turn)
	{
		switch( action_request.request )
		{
			case ActionRequest::CollectCoin:
			{
				if(!CanAquireMoreResources(turn))
					return false;

				if(CoinStack* coin_stack = GetComponent(CoinStack, action_request.target))
				{
					Coin::Type coin_type = coin_stack->coinType;

					// if we have collect 2 different coins, we cannot collect another of those of those
					// it must be a different coin
					std::vector<Coin::Type> types;
					for( u32 i = 0; i < Coin::Count; i++ )
					{
						if( turn.collectedCoins[i] > 0 && !Contains<Coin::Type>(types, (Coin::Type)i) )
							types.push_back((Coin::Type)i);
					}

					if(types.size() >= 2 && Contains<Coin::Type>(types, coin_type))
						return false;

					if(coin_stack->remaining > 0)
					{
						// remove from stack
						coin_stack->remaining--;

						turn.collectedCoins[coin_type]++;
						
						// add to inventory
						if(Inventory* inventory = GetComponent(Inventory, turn.entity))
						{
							inventory->coins[coin_type]++;
						}
					}
				}

				break;
			}
			case ActionRequest::AquireCard:
			{
				if(!CanAquireMoreResources(turn))
					return false;

				// cannot get a card if you've already got some coins
				for( u32 i = 0; i < Coin::Count; i++ )
				{
					if( turn.collectedCoins[i] > 0 )
						return false;
				}
				
				if(Card* card = GetComponent(Card, action_request.target))
				{
					if( CanAffordCard(turn, *card) )
					{
						int card_cost[Coin::Count];
						memcpy(card_cost, card->cost, sizeof(int) * (int)Coin::Count);

						Inventory& inventory = GetComponentRef(Inventory, turn.entity);
						int card_power[Coin::Count];
						inventory.GetCardPower(card_power, Coin::Count);

						// reduce cost of the card by the players card power
						for( u32 i = 0; i < Coin::Count; i++ )
						{
							card_cost[i] = Maths::Max( 0, card_cost[i] - card_power[i]);
							inventory.coins[i] -= card_cost[i];

							// return coins to the stack
							if( CoinStack* cs = CoinStack::GetCoinStack((Coin::Type)i) )
								cs->remaining += card_cost[i];

							ASSERT(inventory.coins[i] >= 0, "the player has less than 0 coins, should be impossible");
						}

						inventory.cards.push_back(*card);

						CardRegistry::ReplaceCard( *card );
					}
				}
			}
			default:
			break;
		}

		return true;
	}

	void TurnActionSystem::Update(float dt)
	{
		int turn_index = -1;
        State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(GameState* game_state = dynamic_cast<GameState*>(&state))
		{
			turn_index = game_state->turnIndex;
			int last_turn_index = -1;
			
			std::vector<Entity> turn_order;

			ComponentArray<TurnState>& turn_states =  GetAllComponents(TurnState);
			for( auto iter = turn_states.entityToComponent.begin(); iter != turn_states.entityToComponent.end(); iter++ )
			{
				turn_order.push_back(iter->first);
			}

			if(turn_order.size() == 0)
				return;

			std::sort(turn_order.begin(), turn_order.end(), [](Entity a, Entity b) { 
				TurnState& turn_A = GetComponentRef(TurnState, a);
				TurnState& turn_B = GetComponentRef(TurnState, b);
				return turn_A.initiative < turn_B.initiative;
			});

			bool entity_has_turn = false;
			for (Entity entity : turn_order)
			{
				TurnState& turn = GetComponentRef(TurnState, entity);
			
				ActionRequest* action_request = GetComponent(ActionRequest, entity);

				// not entities turn
				if(turn.turnIndex > turn_index)
				{
					// cant request actions when its not their turn
					if(action_request)
						RemoveComponent(ActionRequest, entity);

					continue;
				}

				if(action_request)
				{
					ExecuteAction(*action_request, turn);
					RemoveComponent(ActionRequest, entity);
				}

				if(!CanAquireMoreResources(turn))
				{
					turn.ResetState();
					turn.turnIndex++;
				}

				entity_has_turn = true;
			}

			if(!entity_has_turn)
			{	
				// purposfully dont increment turn_index variable
				// the next entity should wait until next loop to do its turn, we need to finish any clean up
				game_state->turnIndex++;
			}
		}
	}
}