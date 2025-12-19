#include "pch.h"
#include "CardRegistry.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/Components/UIComponents.h"
#include "System/Files/JSONParser.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/UIEntityBuilder.h"
#include "Graphics/TextureManager.h"

// random
#include "Core/Helpers.h"


using namespace ECS;

namespace CardRegistry
{
	std::vector<Card> s_cardRegistry;

	std::vector<int> s_cardRegistryDrawPile[3];
	std::vector<int> s_cardRegistryDiscard[3];

	void Build(const char* file, int tier_index)
	{
		using namespace rapidjson;

		BasicString full_path = FileManager::Get()->findFile(FileManager::Configs, file);
		if(full_path.length() == 0)
		{
			DebugPrint(PriorityLevel::Log, "Animation file does not exist: '%s'", file);
			return;
		}

		JSONParser parser(full_path.c_str());
		
		if(!parser.document.IsObject())
		{
			DebugPrint(PriorityLevel::Warning, "Invalid animation document: %s", full_path.c_str());
			return;
		}

		if(parser.document.HasMember("Cards"))
		{
			if(parser.document["Cards"].IsArray())
			{
				const Value::Array& cards = parser.document["Cards"].GetArray();

				int registry_size = (int)s_cardRegistry.size();
				s_cardRegistry.resize(registry_size + cards.Size());

				for( u32 i = 0; i < cards.Size(); i++ )
				{
					const Value& value = cards[i];
					const int registry_index = registry_size + i;
					Card& card = s_cardRegistry[registry_index];

					StringBuffer32 label = value["colour"].GetString();
					Colour::Type type = Colour::s_stringToType.at( label );
					card.colour = type;
					card.power[card.colour] = 1;
					card.cardRegistryIndex = registry_index;
					card.tier = tier_index;
					card.points = value["points"].GetInt();

					const Value::ConstArray& cost = value["cost"].GetArray();
					for( u32 c = 0; c < cost.Size(); c++ )
					{
						card.cost[c] = cost[c].GetInt();
					}

					s_cardRegistryDrawPile[tier_index].push_back(registry_index);
				}
			}
		}
	}

	const Card* LookupCard(int index)
	{
		if(index < 0 || index >= s_cardRegistry.size())
			return nullptr;

		return &s_cardRegistry[index];
	}

	static void GetCard(Card& card, int index)
	{
		if(index >= 0 && index < s_cardRegistry.size())
		{
			CopyComponent(card, s_cardRegistry[index]);
		
			// update the colour
			Sprite& sprite = GetComponentRef(Sprite, card.entity);
			sprite.colourMod = SColour( Colour::s_typeToColour.at(card.colour) );

			if(HasComponent(UIButton, card.entity ))
				card.RegenerateChildDisplays();
		}
	}

	static int PickRandomIndex(int tier)
	{
		std::vector<int> indexes;
		for( int i = 0; i < s_cardRegistry.size(); i++ )
		{
			if(s_cardRegistry[i].tier == tier)
				indexes.push_back(i);
		}

		int random_index = Maths::randomNumberBetween( 0, (int)indexes.size());
		return indexes[random_index];
	}
	
	void DiscardCard(Entity entity)
	{
		const Card& card = GetComponentRef(Card, entity);
		
		// place into discard pile
		std::vector<int>& discard_pile = s_cardRegistryDiscard[card.tier];
		discard_pile.push_back(card.cardRegistryIndex);

		if(Card* card = GetComponent(Card, entity))
		{
			RemoveComponent(Card, entity);
		}
		
		// remove the child displays
		DestroyChildren(entity);
		
		Sprite& sprite = GetComponentRef(Sprite, entity);
		sprite.disabled = true;
	}

	
	void DrawRandomCard(Entity entity, int tier)
	{
		//if(s_cardRegistryDrawPile.size() == 0)
		//{
		//	SetupDrawPile();
		//}

		
		std::vector<int>& draw_pile = s_cardRegistryDrawPile[tier];
		if(draw_pile.size() > 0)
		{
			int random_index = Maths::randomNumberBetween( 0, (int)draw_pile.size());
			int random_registry_index = draw_pile[random_index];

			DrawCard(entity, random_registry_index);
		}

		//std::vector<int> indexes;
		//for( int i = 0; i < s_cardRegistryDrawPile.size(); i++ )
		//{
		//	int registry_index = s_cardRegistryDrawPile[i];
		//	if(s_cardRegistry[registry_index].tier == tier)
		//		indexes.push_back(i);
		//}

		//if(indexes.size() > 0)
		//{
		//	int random_index = Maths::randomNumberBetween( 0, (int)indexes.size());
		//	int random_registry_index = s_cardRegistryDrawPile[random_index];

		//	DrawCard(entity, random_registry_index);
		//}
	}

	void DrawCard(Entity entity, int index)
	{
		if(entity != EntityInvalid)
		{
			if(!HasComponent(Card, entity))
			{
				Card& new_card = AddComponent(Card, entity);
				CardRegistry::GetCard(new_card, index);

				Sprite& sprite = GetComponentRef(Sprite, entity);
				sprite.disabled = false;

				std::vector<int>& draw_pile = s_cardRegistryDrawPile[new_card.tier];
				for( auto iter = draw_pile.begin(); iter != draw_pile.end(); iter++ )
				{
					if(*iter == new_card.cardRegistryIndex)
					{
						draw_pile.erase(iter);
						break;
					}
				}
			}
		}
	}
}