#include "pch.h"
#include "PlayerTurn.h"

#include "Debugging/ImGui/ImGuiMenu.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Objects/CardBoard.h"
#include "Entities/Registries/CardRegistry.h"
#include "Entities/Registries/ResourceBank.h"
#include "Game/States/GameState.h"
#include "Input/InputManager.h"

using namespace ECS;

static void TakeCard(Entity entity, const Card& card)
{
	Inventory& inventory = GetComponentRef(Inventory, entity);
	int card_power[Colour::Count];
	inventory.GetCardPower(card_power);

	// reduce cost of the card by the players card power
	for( u32 i = 0; i < Colour::Count; i++ )
	{
		int cost = card.Cost(i);

		Colour::Type type = (Colour::Type)i;
		int card_cost = Maths::Max( 0, card.Cost(i) - card_power[i]);

		if (type == Colour::Black)
		{
			int diff = card_cost - inventory.coins[type];
			if (diff == 1)
			{
				// deal 5 damage to ourself to reduce the cost by 1
				if (inventory.OwnsRelic("LifeForCost_Black"))
				{
					card_cost -= 2;

					Damage& damage = AddComponent(Damage, entity);
					damage.value = 5.0f;
				}
			}
		}

		// returning coins
		int amount = card_cost;
		ReturnCoinsToStack(entity, type, amount);

		TurnState& turn = GetComponentRef(TurnState, entity);
		turn.collectedCoins[type] -= amount;
	}

	TurnState& turn = GetComponentRef(TurnState, entity);
	turn.collectedCardRegistryIndex = card.registryIndex;

	inventory.cards.push_back(card.registryIndex);

	TriggerGameEvent(GameEvent::CardObtained, card.entity);

	bool triggered_spell = TriggerCard(card.entity, entity);
	if (triggered_spell)
	{
		turn.endTurnCooldownSecs = 2.0f;
	}
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

			int amount = 1;
			TakeCoinsFromStack(turn.entity, coin_type, amount);
			turn.collectedCoins[coin_type] += amount;

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
					int amount = turn.collectedCoins[type];
					ReturnCoinsToStack(turn.entity, type, amount);
					turn.collectedCoins[type] -= amount;
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
	if(DebugMenu::GetSharedState().turnLogActive)
	{
		std::vector<BasicString>& turn_log = DebugMenu::GetSharedState().turnLog;

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

		if(const Card* collected_card = CardRegistry::LookupCard(turn.collectedCardRegistryIndex))
		{
			const char* colour = Colour::s_typeToString.at(collected_card->colour).c_str();
			snprintf(buffer, length, "\tCard Tier: %d, Damage: %d, Colour: %s, Cost: ", collected_card->tier, collected_card->damage, colour);
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

	RestockDiscardedCards();

	turn.turnIndex++;
	turn.ResetState();
}