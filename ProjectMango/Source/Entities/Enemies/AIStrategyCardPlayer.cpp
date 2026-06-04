#include "pch.h"
#include "AIStrategyCardPlayer.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Entities/Registries/ResourceBank.h"

namespace AIStrategy
{
	using namespace ECS;

	static void CardsSortedByTier(std::vector<const Card*>& sorted_cards_by_tier)
	{
		ComponentArray<Card>& cards =  GetAllComponents(Card);
		for( auto iter = cards.entityToComponent.begin(); iter != cards .entityToComponent.end(); iter++ )
		{ 
			const Card& card = cards.GetComponentByIndex(iter->second);
			sorted_cards_by_tier.push_back(&card);
		}

		// sort by highest tier first
		std::sort(sorted_cards_by_tier.begin(), sorted_cards_by_tier.end(), [](const Card* a, const Card* b) { 
			return a->tier > b->tier;
		});
	}

	static Colour::Type CollectAnyCoinTowardsCard(const Card* card, int* buying_power)
	{
		int remaining_cost[Colour::Count];
		std::vector<Colour::Type> colours;
		for( u32 i = 0; i < Colour::Count; i++ )
		{
			remaining_cost[i] = Maths::Max( card->Cost(i) - buying_power[i], 0 );
			if(remaining_cost[i] > 0)
				colours.push_back((Colour::Type)i);
		}

		if(colours.size() == 0)
		{
			return (Colour::Type)Maths::randomNumberBetween(0, Colour::Count);
		}
		else
		{ 
			int random_colour = 0;
			random_colour = Maths::randomNumberBetween(0, (int)colours.size());
			return colours[random_colour];
		}
	}

	static const Card* GetCheapestCard(Entity entity, const std::vector<const Card*>& cards)
	{
		struct RemainingCost
		{
			const Card* card;
			int remaining;
		};
		std::vector<RemainingCost> sorted_cards_by_cost;

		// buy the first card we can afford
		const Inventory* inventory = GetComponent(Inventory, entity);

		int buying_power[Colour::Count];
		inventory->GetBuyingPower(buying_power);

		for( const Card* card : cards )
		{
			int total_remaining = 0;
			int remaining_cost[Colour::Count];
			for( u32 i = 0; i < Colour::Count; i++ )
			{
				remaining_cost[i] = Maths::Max( card->Cost(i) - buying_power[i], 0);
				total_remaining += remaining_cost[i];
			}

			sorted_cards_by_cost.push_back( {card, total_remaining} );
		}

		// sort by least remaining amount first
		std::sort(sorted_cards_by_cost.begin(), sorted_cards_by_cost.end(), [](const RemainingCost& a, const RemainingCost& b) { 
			return a.remaining < b.remaining;
		});

		return sorted_cards_by_cost.front().card;
	}

	static const Card* GetCheapestCardInTier(Entity entity, int tier)
	{
		// probably dont need to do this
		std::vector<const Card*> cards_by_tier;

		ComponentArray<Card>& cards =  GetAllComponents(Card);
		for( auto iter = cards.entityToComponent.begin(); iter != cards .entityToComponent.end(); iter++ )
		{ 
			const Card& card = cards.GetComponentByIndex(iter->second);
			if(card.tier == tier)
				cards_by_tier.push_back(&card);
		}

		return GetCheapestCard(entity, cards_by_tier);
	}

	static void TakeActionTowardsCard(Entity entity, const Card* card)
	{
		const TurnState& turn = GetComponentRef(TurnState, entity);
		// pick any colour we still need or keep collecting if we already started
		if( !card->CanAfford(entity) || turn.HasAquiredResources() )
		{
			ActionRequest& action_request = AddComponent(ActionRequest, entity);
			action_request.request = ActionRequest::CollectCoin;

			const Inventory* inventory = GetComponent(Inventory, entity);
			int buying_power[Colour::Count];
			inventory->GetBuyingPower(buying_power);

			Colour::Type colour = CollectAnyCoinTowardsCard(card, buying_power);

			// we cant collect the coin we actually want, so pick a random one instead
			if(!turn.CanCollectCoin(colour))
				colour = (Colour::Type)Maths::randomNumberBetween(0, Colour::Count);
				
			// todo removed due to refactor
			//CoinStack& cs = GetCoinStack(Faction::None, (u32)colour);
			//action_request.target = cs.entity; 
		}
		// buy the card
		else
		{
			ActionRequest& action_request = AddComponent(ActionRequest, entity);
			action_request.request = ActionRequest::AquireCard;
			action_request.target = card->entity;

			ASSERT(card->CanAfford(entity), "Buying card AI cannot afford" );
		}
	}

	int GetBestCardTier(Entity entity)
	{
		const Inventory* inventory = GetComponent(Inventory, entity);
		
		int card_power[Colour::Count];
		inventory->GetCardPower(card_power);

		int total_power = 0;
		for( u32 i = 0; i < Colour::Count; i++ )
		{
			total_power += card_power[i];
		}

		int best_tier = 0;
		if(total_power >= 5)
			best_tier = 1;
		else if(total_power >= 10)
			best_tier = 2;

		return best_tier;
	}

	void BuyBestCard(Entity entity)
	{
		int tier = GetBestCardTier(entity);
		const Card* card = GetCheapestCardInTier(entity, tier);
		TakeActionTowardsCard(entity, card);
	}

	// collect coins until we can buy the cheapest card
	void BuyCheapestCard(Entity entity)
	{
		const ComponentArray<Card>& cards =  GetAllComponents(Card);
		std::vector<const Card*> all_cards;
		for( auto iter = cards.entityToComponent.begin(); iter != cards .entityToComponent.end(); iter++ )
		{ 
			const Card& card = cards.GetComponentByIndex(iter->second);
			all_cards.push_back(&card);
		}

		const Card* target_card = GetCheapestCard(entity, all_cards);
		TakeActionTowardsCard(entity, target_card);
	}

	// mostly random, really bad AI
	void TakeRandomAction(Entity entity)
	{
		// 0 = get more coins, 1 = try buy a card
		int random_action = Maths::randomNumberBetween(0,2);

		// get a coin
		if(random_action == 0)
		{
			ActionRequest& action_request = AddComponent(ActionRequest, entity);
			action_request.request = ActionRequest::CollectCoin;

			// pick a random colour
			int random_colur = Maths::randomNumberBetween(0, Colour::Count);

			// todo removed due to refactor	
			//CoinStack& cs = GetCoinStack(Faction::None, random_colur);
			//action_request.target = cs.entity; 
		}
		// try buy a card
		else
		{
			std::vector<const Card*> sorted_cards_by_tier;
			CardsSortedByTier(sorted_cards_by_tier);

			// buy the first card we can afford
			for( const Card* card : sorted_cards_by_tier )
			{
				if(card->CanAfford(entity))
				{
					ActionRequest& action_request = AddComponent(ActionRequest, entity);
					action_request.request = ActionRequest::AquireCard;
					action_request.target = card->entity;

					break;
				}
			}
		}
	}
}