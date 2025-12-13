#include "pch.h"
#include "TurnActionSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Core/Helpers.h"
#include "Entities/CardRegistry.h"

namespace ECS
{
	bool CanAffordCard(TurnState& turn, Card& card)
	{
		if(Inventory* inventory = GetComponent(Inventory, turn.entity))
		{
			int buying_power[Colour::Count];
			inventory->GetBuyingPower(buying_power, Colour::Count);
			for( u32 i = 0; i < Colour::Count; i++ )
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
				if(!turn.CanAquireMoreResources())
					return false;

				if(CoinStack* coin_stack = GetComponent(CoinStack, action_request.target))
				{
					Colour::Type coin_type = coin_stack->colourType;

					// if we have collect 2 different coins, we cannot collect another of those of those
					// it must be a different coin
					std::vector<Colour::Type> types;
					for( u32 i = 0; i < Colour::Count; i++ )
					{
						if( turn.collectedCoins[i] > 0 && !Contains<Colour::Type>(types, (Colour::Type)i) )
							types.push_back((Colour::Type)i);
					}

					if(types.size() >= 2 && Contains<Colour::Type>(types, coin_type))
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
				if(!turn.CanAquireMoreResources())
					return false;

				// cannot get a card if you've already got some coins
				for( u32 i = 0; i < Colour::Count; i++ )
				{
					if( turn.collectedCoins[i] > 0 )
						return false;
				}
				
				if(Card* card = GetComponent(Card, action_request.target))
				{
					if( CanAffordCard(turn, *card) )
					{
						int card_cost[Colour::Count];
						memcpy(card_cost, card->cost, sizeof(int) * (int)Colour::Count);

						Inventory& inventory = GetComponentRef(Inventory, turn.entity);
						int card_power[Colour::Count];
						inventory.GetCardPower(card_power, Colour::Count);

						// reduce cost of the card by the players card power
						for( u32 i = 0; i < Colour::Count; i++ )
						{
							card_cost[i] = Maths::Max( 0, card_cost[i] - card_power[i]);
							inventory.coins[i] -= card_cost[i];

							// return coins to the stack
							if( CoinStack* cs = CoinStack::GetCoinStack((Colour::Type)i) )
							{
								cs->remaining += card_cost[i];
								
								turn.collectedCoins[i] = -card_cost[i];
							}

							ASSERT(inventory.coins[i] >= 0, "the player has less than 0 coins, should be impossible");
						}

						CopyComponent(turn.collectedCard, *card);
						turn.collectedCardSource = card->entity;

						inventory.cards.push_back(card->cardRegistryIndex); 

						CardRegistry::RemoveCard(card->entity);
					}
				}

				break;
			}
			case ActionRequest::UndoTurn:
			{
				// return collected coins
				for( u32 i = 0; i < Colour::Count; i++ )
				{
					Colour::Type type = (Colour::Type)i;

					if( turn.collectedCoins[type] != 0 )
					{
						// return to coin stack
						CoinStack* coin_stack = CoinStack::GetCoinStack(type);
						coin_stack->remaining += turn.collectedCoins[type];

						// remove from inventory
						if(Inventory* inventory = GetComponent(Inventory, turn.entity))
						{
							inventory->coins[type] -= turn.collectedCoins[type];
						}

						turn.collectedCoins[i] = 0;
					}
				}

				// return collected card
				if(turn.collectedCardSource != EntityInvalid)
				{
					CardRegistry::DrawCard(turn.collectedCardSource, turn.collectedCard.cardRegistryIndex);

					// remove from inventory
					if(Inventory* inventory = GetComponent(Inventory, turn.entity))
					{
						Erase(inventory->cards, turn.collectedCard.cardRegistryIndex);
					}
				}
					
				turn.ResetState();

				break;
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

				// end turn
				bool can_end_turn = turn.canEndTurn || game_state->autoConfirmTurn;
				if(can_end_turn && !turn.CanAquireMoreResources())
				{
					int random_card_index = CardRegistry::PickRandomIndex(turn.collectedCard.tier);
					CardRegistry::DrawCard( turn.collectedCardSource, random_card_index );

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