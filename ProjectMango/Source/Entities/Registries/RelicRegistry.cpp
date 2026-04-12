#include "pch.h"
#include "RelicRegistry.h"
#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
//#include "ECS/Components/ComponentHelpers.h"

using namespace ECS;

namespace RelicRegistry
{
	std::vector<Relic> s_relicRegistry;

	static void ReducedCardCost(const Relic& relic, ECS::Entity entity)
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
			//card->cost[(int)colour_to_remove]--;

			//int target_colour_hex = Colour::s_typeToColour.at(colour_to_remove).GetHex();

			//// dodgy bit, look through all the costs and find one to recolour (the largest y position?)
			//std::vector<Entity> children_cost_icons;
			//GetChildren(entity, children_cost_icons);

			//Entity best_entity = EntityInvalid;
			//float y_position = FLT_MAX;
			//for( u32 i = 0; i < children_cost_icons.size(); i++ )
			//{
			//	// turn one purple
			//	if(Sprite* sprite = GetComponent(Sprite, children_cost_icons[i]))
			//	{
			//		SColour colour = sprite->params.colourMod;
			//		//colour.setOpacity(1.0f); // make it simple
			//		if(colour.GetHex() == target_colour_hex)
			//		{

			//			VectorF child_position = GetPosition(children_cost_icons[i]);
			//			if(child_position.y < y_position)
			//			{
			//				y_position = child_position.y;
			//				best_entity = children_cost_icons[i];
			//			}

			//			//sprite->SetTexture("cost_removed");
			//			//sprite->params.colourMod = SColour(SColour::Purple);
			//			//break;
			//		}
			//	}
			//}

			//if(best_entity != EntityInvalid)
			//{
			//	// turn one purple
			//	if(Sprite* sprite = GetComponent(Sprite, best_entity))
			//	{
			//		sprite->SetTexture("cost_removed");
			//		sprite->params.colourMod = SColour(SColour::Purple);
			//	}
			//}
		}
	}

	void PopulateRegistry()
	{
		s_relicRegistry.push_back(Relic());

		Relic& relic = s_relicRegistry.back();
		relic.id = "Reduced Card Cost";
		relic.description = "Reduces the cost of all specified coloured cards by 1 random colour";
		relic.trigger = GameEvent::CardDrawn;
		relic.colour = Colour::Black;
		relic.effectFn = ReducedCardCost;
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