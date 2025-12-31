#include "pch.h"
#include "TurnActionSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Core/Helpers.h"
#include "Entities/CardRegistry.h"
#include "Entities/MonsterRegistry.h"
#include "Game/FrameRateController.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "Entities/ResourceBank.h"

namespace ECS
{
	char turnLog[256] = { 0 };

	// +amount = take coins, -amount = return coins
	static void TakeCoins(Entity entity, Colour::Type colour, int amount)
	{
		Inventory& inventory = GetComponentRef(Inventory, entity);
		inventory.coins[colour] += amount;

		// return coins to the stack
		CoinStack& cs = GetCoinStack(Faction::None, (u32)colour);
		cs.remaining -= amount;

		TurnState& turn = GetComponentRef(TurnState, entity);
		turn.collectedCoins[colour] += amount;
	}

	static void TakeCard(Entity entity, const Card& card)
	{
		int card_cost[Colour::Count];
		memcpy(card_cost, card.cost, sizeof(int) * (int)Colour::Count);

		Inventory& inventory = GetComponentRef(Inventory, entity);
		int card_power[Colour::Count];
		inventory.GetCardPower(card_power, Colour::Count);

		// reduce cost of the card by the players card power
		for( u32 i = 0; i < Colour::Count; i++ )
		{
			Colour::Type type = (Colour::Type)i;
			card_cost[i] = Maths::Max( 0, card_cost[i] - card_power[i]);

			// returning coins
			TakeCoins(entity, type, -card_cost[i]);
		}

		TurnState& turn = GetComponentRef(TurnState, entity);
		turn.collectedCardRegIndex = card.registryIndex;
		turn.collectedCardSource = card.entity;

		inventory.cards.push_back(card.registryIndex); 

		if(card.monsterRegistryIndex != -1)
		{
			// spawn
			Entity spawn_requst = CreateEntity("SpawnRequest");
			SpawnRequest& sr = AddComponent(SpawnRequest, spawn_requst);
			sr.frameTime = FrameRateController::Get().frameCount;
			sr.emd.id = MonsterRegistry::GetMonster(card.monsterRegistryIndex);
			sr.owner = turn.entity;
			sr.cardRegistryIndex = card.registryIndex;
		}

		// destroys all children
		CardRegistry::DiscardCard(card.entity);
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
						TakeCoins(turn.entity, coin_type, 1);
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
					if( card->CanAfford(turn.entity) )
					{
						TakeCard(turn.entity, *card);
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
						TakeCoins(turn.entity, type, -turn.collectedCoins[type]);
					}
				}

				// return collected card
				if(turn.collectedCardSource != EntityInvalid)
				{
					CardRegistry::DrawCard(turn.collectedCardSource, turn.collectedCardRegIndex);

					// remove from inventory
					if(Inventory* inventory = GetComponent(Inventory, turn.entity))
					{
						Erase(inventory->cards, turn.collectedCardRegIndex);
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

	// gets the turn state it should be now
	TurnState* GetCurrentTurnState()
	{
		std::vector<Entity> turn_order;

		ComponentArray<TurnState>& turn_states =  GetAllComponents(TurnState);
		for( auto iter = turn_states.entityToComponent.begin(); iter != turn_states.entityToComponent.end(); iter++ )
		{
			turn_order.push_back(iter->first);
		}

		std::sort(turn_order.begin(), turn_order.end(), [](Entity a, Entity b) { 
			TurnState& turn_A = GetComponentRef(TurnState, a);
			TurnState& turn_B = GetComponentRef(TurnState, b);
			return turn_A.initiative < turn_B.initiative;
		});

        State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		if(GameState* game_state = dynamic_cast<GameState*>(&state))
		{
			for (Entity entity : turn_order)
			{
				TurnState& turn = GetComponentRef(TurnState, entity);

				// not entities turn
				if(turn.turnIndex > game_state->turnIndex)
				{
					continue;
				}
				return &turn;
			}
		}

		return nullptr;
	}

	void TurnActionSystem::Update(float dt)
	{
		State& state = GameData::Get().systemStateManager->mStates.getActiveState();
		GameState* game_state = dynamic_cast<GameState*>(&state);

		if(TurnState* turn = GetCurrentTurnState())
		{
			if(!turn->isActiveTurn)
			{			
				turn->isActiveTurn = true;
				turn->ResetState();
			}

			Entity entity = turn->entity;

			if(ActionRequest* action_request = GetComponent(ActionRequest, entity))
			{
				ExecuteAction(*action_request, *turn);
				RemoveComponent(ActionRequest, entity);
			}

			// end turn
			bool can_end_turn = turn->canEndTurn || game_state->autoConfirmTurn;
			if(can_end_turn && !turn->CanAquireMoreResources())
			{
				if(DebugMenu::GetState().turnLogActive)
				{
					std::vector<BasicString>& turn_log = DebugMenu::GetState().turnLog;

					char header[32];
					snprintf(header, 32, "\n%s Turn %d", Faction::GetTeam(entity) == Faction::Player ? "Player" : "Enemy", turn->turnIndex );
					turn_log.emplace_back(BasicString(header));

					const int length = 512;
					char buffer[length] = { 0 };

					for( u32 i = 0; i < Colour::Count; i++ )
					{
						if(turn->collectedCoins[i] > 0)
						{
							snprintf(buffer, length, "\t%s: %d", Colour::s_typeToString.at((Colour::Type)i).c_str(), turn->collectedCoins[i] );
							turn_log.emplace_back( BasicString( buffer ) );
						}
					}
					
					if(const Card* collected_card = CardRegistry::LookupCard(turn->collectedCardRegIndex))
					{
						const char* power = nullptr;
						for( u32 i = 0; i < Colour::Count; i++ )
						{
							if(collected_card->power[i] > 0)
							{
								power = Colour::s_typeToString.at((Colour::Type)i).c_str();
								break;
							}
						}

						snprintf(buffer, length, "\tCard Tier: %d, Points: %d, Power: %s, Cost: ", collected_card->tier, collected_card->points, power );
						for( u32 i = 0; i < Colour::Count; i++ )
						{
							if(collected_card->cost[i] > 0)
							{
								int spent_coins = turn->collectedCoins[i];

								char cost[32];
								snprintf(cost, 32, " %s: %d(%d),", Colour::s_typeToString.at((Colour::Type)i).c_str(), collected_card->cost[i], spent_coins );
								strncat(buffer, cost, strlen(cost));
							}
						}

						turn_log.emplace_back( BasicString( buffer ) );
					}
				}

				if(const Card* collected_card = CardRegistry::LookupCard(turn->collectedCardRegIndex))
				{
					// redraw any cards we removed 
					//int random_card_index = CardRegistry::PickRandomIndex(collected_card->tier);
					CardRegistry::DrawRandomCard( turn->collectedCardSource, collected_card->tier );
				}

				// return coins when over limit (10)
				if(Inventory* inventory = GetComponent(Inventory, turn->entity))
				{
					int coin_count = 0;
					for( u32 i = 0; i < Colour::Count; i++ )
					{
						Colour::Type c_type = (Colour::Type)i;
						coin_count += inventory->coins[c_type];
					}

					while(coin_count > 10)
					{
						int largest_amount = 0;
						Colour::Type largest_pile = Colour::Count;
											
						for( u32 i = 0; i < Colour::Count; i++ )
						{
							Colour::Type c_type = (Colour::Type)i;
							if(inventory->coins[c_type] > largest_amount)
							{
								largest_amount = inventory->coins[c_type];
								largest_pile = c_type;
							}
						}

						TakeCoins(entity, largest_pile, -1 );
						coin_count--;

					}
				}

				turn->isActiveTurn = false;
				turn->turnIndex++;
			}
		}
		else
		{
			if(entities.size() > 0)
			{	
				// purposfully dont increment turn_index variable
				// the next entity should wait until next loop to do its turn, we need to finish any clean up
				game_state->turnIndex++;
			}
		}
	}
}