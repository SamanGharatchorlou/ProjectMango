#include "pch.h"
#include "RelicRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Core/Helpers.h"
#include "Entities/Registries/CardRegistry.h"
#include "Entities/Registries/ResourceBank.h"
#include "Entities/Objects/CardBoard.h"

using namespace ECS;

namespace RelicRegistry
{
	std::vector<Relic> s_relicRegistry;

	const std::vector<Relic>& GetRelicRegistry()
	{
		return s_relicRegistry;
	}

	static void DiscountCardCost(const Relic& relic, ECS::Entity entity)
	{
		Card* card = GetComponent(Card, entity);
		if (!card || card->colour != relic.colour)
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
						if ( card->colour == relic.colour )
						{
							draw_pile[k].weight *= 2;
						}
					}
				}
			}
		}
	}

	static void IncreaseRedCardDamage(const Relic& relic, ECS::Entity entity)
	{
		Card* card = GetComponent(Card, entity);
		if (!card || card->colour != Colour::Red)
			return;

		if (card->damage > 0)
			card->damage += 5;
	}

	static void GainLife(const Relic& relic, ECS::Entity entity)
	{
		Card* card = GetComponent(Card, entity);
		if (!card || card->colour != Colour::Green)
			return;

		if (Health* hp = GetComponent(Health, Faction::GetPlayer()))
		{
			hp->Heal(5.0f);
		}
	}

	static void GainWhiteMana(const Relic& relic, ECS::Entity entity)
	{
		Card* card = GetComponent(Card, entity);
		if (!card || card->colour != Colour::White)
			return;

		int amount = 1;
		TakeCoinsFromStack(Faction::GetPlayer(), Colour::White, amount);
	}

	static void RedrawNonBlueCard(const Relic& relic, ECS::Entity entity)
	{
		Card* blue_card = GetComponent(Card, entity);
		if (!blue_card || blue_card->colour != Colour::Blue)
			return;

		CardBoard* board = GetOnlyComponent(CardBoard);
		if (!board)
			return;

		std::vector<Entity> valid_redraw_entities;
		for (u32 x = 0; x < board->cards.xCount(); x++)
		{
			for (u32 y = 0; y < board->cards.yCount(); y++)
			{
				VectorI index(x, y);

				// skip the card index we've just interacted with
				if (blue_card->boardIndex == index)
					continue;

				Entity card_entity = board->cards.get(index);
				if (const Card* card = GetComponent(Card, card_entity))
				{
					if (card->colour != Colour::Blue)
						valid_redraw_entities.push_back(card_entity);
				}
			}
		}

		if (valid_redraw_entities.size() == 0)
			return;

		int rnd_index = Maths::randomNumberBetween(0, (int)valid_redraw_entities.size());
		RedrawCard(valid_redraw_entities[rnd_index]);
	}

	void PopulateRegistry()
	{
		s_relicRegistry.clear();

		// DiscountCardCost
		for (u32 i = 0; i < Colour::Count; i++)
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			Colour::Type colour = (Colour::Type)i;
			const char* colour_string = Colour::s_typeToString.at(colour).c_str();

			char id_buffer[64];
			snprintf(id_buffer, 64, "DiscountCardCost_%s", colour_string);

			char desc_buffer[128];
			snprintf(desc_buffer, 128, "Reduces the cost of all %s cards by 1 random colour", colour_string);

			relic.id = id_buffer;
			relic.description = desc_buffer;
			relic.colour = colour;
			relic.trigger = GameEvent::CardDrawn;
			relic.effectFn = DiscountCardCost;
		}

		// IncreaseDrawChance
		for (u32 i = 0; i < Colour::Count; i++)
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			Colour::Type colour = (Colour::Type)i;
			const char* colour_string = Colour::s_typeToString.at(colour).c_str();

			char id_buffer[64];
			snprintf(id_buffer, 64, "IncreaseDrawChance_%s", colour_string);

			char desc_buffer[128];
			snprintf(desc_buffer, 128, "Increase the draw chance of %s cards", colour_string);

			relic.id = id_buffer;
			relic.description = desc_buffer;
			relic.colour = colour;
			relic.trigger = GameEvent::DrawPileBuilt;
			relic.effectFn = IncreaseColourDrawChance;
		}

		// BlackLifeForCost
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			char buffer[128];
			snprintf(buffer, 128, "May consume 5 life to reduce cost of card by 2 black");

			relic.id = "LifeForCost_Black";
			relic.description = buffer;
			relic.colour = Colour::Black;

			// no trigger or ever - manually handled
			relic.trigger = GameEvent::None;
			relic.effectFn = nullptr;
		}

		// GreenGainLife
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			char buffer[128];
			snprintf(buffer, 128, "Gain 5 life when you obtain a green card");

			relic.id = "GainLife_Green";
			relic.description = buffer;
			relic.colour = Colour::Green;
			relic.trigger = GameEvent::CardObtained;
			relic.effectFn = GainLife;
		}

		// RedBonusDamage
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			char buffer[128];
			snprintf(buffer, 128, "Increases red card damage by 5");

			relic.id = "BonusDamage_Red";
			relic.description = buffer;
			relic.colour = Colour::Red;
			relic.trigger = GameEvent::CardDrawn;
			relic.effectFn = IncreaseRedCardDamage;
		}

		// BlueRedraw
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			char buffer[128];
			snprintf(buffer, 128, "Redraw 1 random non-blue card when you obtain a blue card");

			relic.id = "RedrawCard_Blue";
			relic.description = buffer;
			relic.colour = Colour::Blue;
			relic.trigger = GameEvent::CardObtained;
			relic.effectFn = RedrawNonBlueCard;
		}

		// GainWhiteMana
		{
			s_relicRegistry.push_back(Relic());
			Relic& relic = s_relicRegistry.back();

			char buffer[128];
			snprintf(buffer, 128, "Gain 1 white mana when you obtain a white card");

			relic.id = "GainMana_White";
			relic.description = buffer;
			relic.colour = Colour::White;
			relic.trigger = GameEvent::CardObtained;
			relic.effectFn = GainWhiteMana;
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