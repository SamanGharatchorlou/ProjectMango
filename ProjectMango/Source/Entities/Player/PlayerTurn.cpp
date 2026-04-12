#include "pch.h"
#include "PlayerTurn.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"

#include "Game/States/GameState.h"
#include "Core/Helpers.h"
#include "Entities/Registries/CardRegistry.h"
#include "Entities/Registries/MonsterRegistry.h"
#include "Game/FrameRateController.h"
#include "Debugging/ImGui/ImGuiMainWindows.h"
#include "Entities/Registries/ResourceBank.h"

#include "Input/InputManager.h"

using namespace ECS;

char turnLog[256] = { 0 };

// +amount = take coins, -amount = return coins
static void TakeCoins(Entity entity, Colour::Type colour, int amount)
{
	Inventory& inventory = GetComponentRef(Inventory, entity);
	inventory.coins[colour] = Maths::clamp(inventory.coins[colour] + amount, 0, 5);

	// return coins to the stack
	CoinStack& cs = GetCoinStack(Faction::None, (u32)colour);
	cs.remaining -= amount;
	cs.remaining = Maths::clamp(cs.remaining, 0, cs.capacity);

	TurnState& turn = GetComponentRef(TurnState, entity);
	turn.collectedCoins[colour] += amount;
}

static void TakeCard(Entity entity, const Card& card)
{
	//int card_cost[Colour::Count];

	//memcpy(card_cost, card.cost, sizeof(int) * (int)Colour::Count);

	Inventory& inventory = GetComponentRef(Inventory, entity);
	int card_power[Colour::Count];
	inventory.GetCardPower(card_power);

	// reduce cost of the card by the players card power
	for( u32 i = 0; i < Colour::Count; i++ )
	{
		int cost = card.Cost(i);

		Colour::Type type = (Colour::Type)i;
		int card_cost = Maths::Max( 0, card.Cost(i) - card_power[i]);

		// returning coins
		TakeCoins(entity, type, -card_cost);
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
		sr.emd.data.strings["Id"] = MonsterRegistry::GetMonster(card.monsterRegistryIndex);
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
			const CoinStack& coin_stack = GetComponentRef(CoinStack, action_request.target);
			Colour::Type coin_type = coin_stack.colourType;
			if(!turn.CanCollectCoin(coin_type))
				return false;

			TakeCoins(turn.entity, coin_type, 1);

			break;
		}
		case ActionRequest::AquireCard:
		{
			if(turn.HasAquiredResources())
				return false;
				
			if(Card* card = GetComponent(Card, action_request.target))
			{
				if( !card->CanAfford(turn.entity) )
					return false;
				
				TakeCard(turn.entity, *card);
			}

			break;
		}
		case ActionRequest::ReturnCoins:
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
					
			turn.ResetState();
			break;
		}
		default:
		break;
	}

	return true;
}

void PlayerTurn::Update(ECS::TurnState& turn)
{
	Entity entity = turn.entity;

	if(ActionRequest* action_request = GetComponent(ActionRequest, entity))
	{
		bool did_execute = ExecuteAction(*action_request, turn);

		if(!did_execute && Faction::GetPlayer() == entity)
		{
			Jiggler& jiggle = AddComponent(Jiggler, action_request->target);
			jiggle.amplitude = 8.0f;
			jiggle.frequency = 30.0f;
			jiggle.decayTime = 4.0f;
			jiggle.undisturbedLoops = 1;
		}

		RemoveComponent(ActionRequest, entity);
	}
	
	turn.canEndTurn = !turn.CanAquireMoreResources();

	GameState* game_state = GameState::GetActive();
	if(game_state->autoConfirmTurn)
		turn.tryEndTurn = game_state->autoConfirmTurn;

	InputManager* input = InputManager::Get();
	if(input->isPressed(Button::Space))
	{
		turn.tryEndTurn = true;
		turn.canEndTurn = true;
	}

	if(turn.tryEndTurn)
		turn.endTurnCooldownSecs = 0.2f;
}


void PlayerTurn::OnEndTurn(ECS::TurnState& turn)
{
	if(DebugMenu::GetState().turnLogActive)
	{
		std::vector<BasicString>& turn_log = DebugMenu::GetState().turnLog;

		char header[32];
		snprintf(header, 32, "\n%s Turn %d", Faction::GetTeam(turn.entity) == Faction::Player ? "Player" : "Enemy", turn.turnIndex );
		turn_log.emplace_back(BasicString(header));

		const int length = 512;
		char buffer[length] = { 0 };

		for( u32 i = 0; i < Colour::Count; i++ )
		{
			if(turn.collectedCoins[i] > 0)
			{
				snprintf(buffer, length, "\t%s: %d", Colour::s_typeToString.at((Colour::Type)i).c_str(), turn.collectedCoins[i] );
				turn_log.emplace_back( BasicString( buffer ) );
			}
		}
					
		if(const Card* collected_card = CardRegistry::LookupCard(turn.collectedCardRegIndex))
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
				if(collected_card->Cost(i) > 0)
				{
					int spent_coins = turn.collectedCoins[i];

					char cost[32];
					snprintf(cost, 32, " %s: %d(%d),", Colour::s_typeToString.at((Colour::Type)i).c_str(), collected_card->Cost(i), spent_coins );
					strncat(buffer, cost, strlen(cost));
				}
			}

			turn_log.emplace_back( BasicString( buffer ) );
		}
	}

	if(const Card* collected_card = CardRegistry::LookupCard(turn.collectedCardRegIndex))
	{
		// redraw any cards we removed
		CardRegistry::DrawRandomCard( turn.collectedCardSource, collected_card->tier );
	}

	turn.turnIndex++;
	turn.ResetState();
}