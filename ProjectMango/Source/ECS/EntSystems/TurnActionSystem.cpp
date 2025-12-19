#include "pch.h"
#include "TurnActionSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"

#include "Game/SystemStateManager.h"
#include "Game/States/GameState.h"
#include "Core/Helpers.h"
#include "Entities/CardRegistry.h"
#include "ECS/Components/Animator.h"

#include "Animations/ConfigReaders.h"
#include "ECS/Components/AIComponents.h"
#include "Game/FrameRateController.h"

namespace ECS
{
	// +amount = take coins, -amount = return coins
	static void TakeCoins(Entity entity, Colour::Type colour, int amount)
	{
		Inventory& inventory = GetComponentRef(Inventory, entity);
		inventory.coins[colour] += amount;

		// return coins to the stack
		CoinStack* cs = CoinStack::GetCoinStack(colour);
		cs->remaining -= amount;

		TurnState& turn = GetComponentRef(TurnState, entity);
		turn.collectedCoins[colour] += amount;
		
		/*
		* This nees a VFX system to work properly
		* 
		Entity cs_entity = cs->entity;

		Animator& animator = AddComponent(Animator, cs_entity);
		
		AnimationReader::BuildAnimatior( animator, "ClinkAnimation" );
		animator.StartAnimation(ActionState::Active);
		*/
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

			//ASSERT(inventory.coins[type] >= 0, "the player has less than 0 coins, should be impossible");
		}

		TurnState& turn = GetComponentRef(TurnState, entity);
		turn.collectedCardRegIndex = card.cardRegistryIndex;
		turn.collectedCardSource = card.entity;

		inventory.cards.push_back(card.cardRegistryIndex); 

		// spawn
		Entity spawn_requst = CreateEntity("SpawnRequest");
		SpawnRequest& sr = AddComponent(SpawnRequest, spawn_requst);
				
		const FrameRateController& frc = FrameRateController::Get();
		sr.frameTime = frc.frameCount;
		if(card.points > 1)
			sr.emd.id = "ShockSweeper";
		else
			sr.emd.id = "BlindingSpider";

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

			//if(turn->attackingMonster != EntityInvalid)
			//{
			//	if(EntityState* state = GetComponent(EntityState, turn->attackingMonster))
			//	{
			//		state->target = game_state->enemy;

			//		// spawn to target

			//		VectorF spawn_pos = GetPosition(state->target);

			//		//FindEnt
			//		SetWorldPosition(turn->attackingMonster, spawn_pos);

			//		ComponentArray<Spawner>& spawners =  GetAllComponents(Spawner);
			//		Entity spawner_entity = spawners.entityToComponent.begin()->first;
			//		Spawner& spawner = GetComponentRef(Spawner, spawner_entity);
			//		spawner.Spawn(turn->attackingMonster);

			//		//EntityState& scharacter_state = GetComponentRef(EntityState, spawner.entityToSpawn);
			//		//state->character->SpawnIn(turn->attackingMonster);

			//		// need to keep this around somewhere?
			//		turn->attackingMonster = EntityInvalid;
			//	}
			//}

			if(ActionRequest* action_request = GetComponent(ActionRequest, entity))
			{
				ExecuteAction(*action_request, *turn);
				RemoveComponent(ActionRequest, entity);
			}

			// end turn
			bool can_end_turn = turn->canEndTurn || game_state->autoConfirmTurn;
			if(can_end_turn && !turn->CanAquireMoreResources())
			{
				if(const Card* collected_card = CardRegistry::LookupCard(turn->collectedCardRegIndex))
				{
					// redraw any cards we removed 
					//int random_card_index = CardRegistry::PickRandomIndex(collected_card->tier);
					CardRegistry::DrawRandomCard( turn->collectedCardSource, collected_card->tier );

					// apply card damage
					if(collected_card->points > 0)
					{
						Entity target = Target::GetValidTarget(entity);
						if(target != EntityInvalid)
						{
							if(Health* health = GetComponent(Health, target))
								health->ApplyDamage((float)collected_card->points);
						}
					}
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