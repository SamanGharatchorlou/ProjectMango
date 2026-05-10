#include "pch.h"
#include "RelicRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "CardRegistry.h"
#include "Core/Helpers.h"

using namespace ECS;

namespace RelicRegistry
{
	std::vector<Relic> s_relicRegistry;

	static void DiscountCardCost(const Relic& relic, ECS::Entity entity)
	{
		if(Card* card = GetComponent(Card, entity))
		{
			// we only card about cards of a certain colour
			if(card->power[relic.colour] == 0)
				return;

			std::vector<Colour::Type> colours;
			for( u32 i = 0; i < Colour::Count; i++ )
			{
				if(card->cost[i] > 0)
					colours.push_back((Colour::Type)i);
			}

			int random_colour_index = Maths::randomNumberBetween(0, (int)colours.size());
			Colour::Type colour_to_remove = colours[random_colour_index];

			card->discount[(int)colour_to_remove]++;
		}
	}

	static void IncreaseColourDrawRate(const Relic& relic, ECS::Entity entity)
	{
		if(Card* card = GetComponent(Card, entity))
		{
			// if its the target colour then we dont want to draw again
			if(card->power[relic.colour] != 0)
				return;

			Entity player = Faction::GetPlayer();
			if(Inventory* inventory = GetComponent(Inventory, player))
				inventory->disabledRelicIds.push_back(relic.id);

			// replace card and draw again
			int tier = card->tier;
			CardRegistry::ReturnCardToDrawPile(entity);
			CardRegistry::DrawRandomCard(entity, tier);

			if(Inventory* inventory = GetComponent(Inventory, player))
				Erase(inventory->disabledRelicIds, relic.id);
		}
	}

	void PopulateRegistry()
	{
		s_relicRegistry.clear();

		s_relicRegistry.push_back(Relic());
		Relic& relic_1 = s_relicRegistry.back();
		relic_1.id = "DiscountCardCost";
		relic_1.description = "Reduces the cost of all specified coloured cards by 1 random colour";
		relic_1.trigger = GameEvent::CardDrawn;
		relic_1.colour = Colour::Black;
		relic_1.effectFn = DiscountCardCost;
		
		s_relicRegistry.push_back(Relic());
		Relic& relic_2 = s_relicRegistry.back();
		relic_2.id = "IncreaseColourDrawRate";
		relic_2.description = "Increase the draw rate of the specified coloured card";
		relic_2.trigger = GameEvent::CardDrawn;
		relic_2.colour = Colour::White;
		relic_2.effectFn = IncreaseColourDrawRate;
	}

	
	Relic* GetRelic(const char* relic_id)
	{
		for( u32 i = 0; i < s_relicRegistry.size(); i++ )
		{
			if(s_relicRegistry[i].id == relic_id)
				return &s_relicRegistry[i];
		}

		return nullptr;
	}
}