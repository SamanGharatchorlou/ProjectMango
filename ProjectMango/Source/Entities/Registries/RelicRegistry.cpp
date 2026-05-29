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

	static void IncreaseColourDrawChance(const Relic& relic, ECS::Entity entity)
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

		for (u32 i = 0; i < Colour::Count; i++)
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			relic.id = "DiscountCardCost";

			Colour::Type colour = (Colour::Type)i;
			const char* colour_string = Colour::s_typeToString.at(colour).c_str();

			char buffer[128];
			snprintf(buffer, 128, "Reduces the cost of all %s cards by 1 random colour", colour_string);
			relic.description = buffer;

			relic.trigger = GameEvent::CardDrawn;
			relic.colour = colour;
			relic.effectFn = DiscountCardCost;
		}

		for (u32 i = 0; i < Colour::Count; i++)
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			relic.id = "IncreaseColourDrawChance";

			Colour::Type colour = (Colour::Type)i;
			const char* colour_string = Colour::s_typeToString.at(colour).c_str();

			char buffer[128];
			snprintf(buffer, 128, "Increase the draw chance of %s cards", colour_string);
			relic.description = buffer;

			relic.trigger = GameEvent::CardDrawn;
			relic.colour = colour;
			relic.effectFn = IncreaseColourDrawChance;
		}
	}
	
	//Relic* GetRelic(const char* relic_id)
	//{
	//	for( u32 i = 0; i < s_relicRegistry.size(); i++ )
	//	{
	//		 todo - this wont work anymore
	//		if(s_relicRegistry[i].id == relic_id)
	//			return &s_relicRegistry[i];
	//	}

	//	return nullptr;
	//}

	ECS::Relic* GetRandomUnobtainedRelic()
	{
		Entity player = Faction::GetPlayer();
		Inventory* inventory = GetComponent(Inventory, player);

		std::vector<Relic*> unobtained_relics;
		for (u32 i = 0; i < s_relicRegistry.size(); i++)
		{
			if (inventory && Contains(inventory->relics, s_relicRegistry[i]))
				continue;

			unobtained_relics.push_back(&s_relicRegistry[i]);
		}

		int random_index = Maths::randomNumberBetween(0, unobtained_relics.size());
		return unobtained_relics[random_index];
	}
}