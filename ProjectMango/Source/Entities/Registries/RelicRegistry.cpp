#include "pch.h"
#include "RelicRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"
#include "Entities/Registries/CardRegistry.h"

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
		if(CardBoard* board = GetComponent(CardBoard, entity))
		{
			for (u32 i = 0; i < Card::c_tiers; i++)
			{
				std::vector<DeckCard>& draw_pile = board->drawPile[i];
				for (u32 k = 0; k < draw_pile.size(); k++)
				{
					if(const Card* card = CardRegistry::LookupCard(draw_pile[k].registryIndex))
					{
						if (card->IsColour(relic.colour) )
						{
							draw_pile[k].weight *= 2;
						}
					}
				}
			}
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
			relic.phase = Relic::Effect;
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

			relic.trigger = GameEvent::DrawPileBuilt;
			relic.colour = colour;
			relic.effectFn = IncreaseColourDrawChance;
			relic.phase = Relic::Selection;
		}
	}

	ECS::Relic* GetRandomUnobtainedRelic()
	{
		std::vector<Relic*> unobtained_relics;
		for (u32 i = 0; i < s_relicRegistry.size(); i++)
		{
			bool obtained = false;

			const ComponentArray<Inventory>& inventories = GetAllComponents(Inventory);
			for (auto [key, value] : inventories.entityToComponent)
			{
				const Inventory& inventory = inventories.GetComponentByIndex(value);
				if (Contains(inventory.relics, s_relicRegistry[i]))
				{
					obtained = true;
					break;
				}
			}

			if (obtained)
				continue;

			unobtained_relics.push_back(&s_relicRegistry[i]);
		}

		if (unobtained_relics.size() == 0)
			return nullptr;

		int random_index = Maths::randomNumberBetween(0, (int)unobtained_relics.size());
		return unobtained_relics[random_index];
	}
}