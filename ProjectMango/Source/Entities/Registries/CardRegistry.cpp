#include "pch.h"
#include "CardRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "System/Files/JSONParser.h"
#include "System/Files/TextFileReader.h"
#include "ECS/EntityCoordinator.h"
#include "MonsterRegistry.h"
#include "SpellRegistry.h"

using namespace ECS;

namespace CardRegistry
{
	std::vector<Card> s_cardRegistry;

	std::vector<int> s_cardRegistryDrawPile[Card::c_tiers];
	std::vector<int> s_cardRegistryDiscard[Card::c_tiers];

	
	void ClearAll()
	{
		s_cardRegistry.clear();

		for( u32 i = 0; i < Card::c_tiers; i++ )
		{
			s_cardRegistryDrawPile[i].clear();
			s_cardRegistryDiscard[i].clear();
		}
	}

	void ReadomFromJson(const char* file, int tier_index)
	{
		using namespace rapidjson;
		
		BasicString file_path;
		FileManager::Get()->FindFile(FileManager::Configs, file, file_path);
		JSONParser parser(file_path.c_str());
		if(!parser.IsValid())
			return;

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
					card.registryIndex = registry_index;
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

	
	void ReadomFromCSV(const char* file, int tier_index)
	{
		BasicString file_path;
		FileManager::Get()->FindFile( FileManager::Configs, file, file_path );
		TextFileReader reader(file_path.c_str());

		std::string line;
		while(std::getline(reader.mFile, line))
		{
			// read a line, each line is a card
			std::stringstream ss(line);

			s_cardRegistry.push_back( Card() );
			Card& card = s_cardRegistry.back();
			card.tier = tier_index;
			card.registryIndex = (int)s_cardRegistry.size() - 1;

			s_cardRegistryDrawPile[tier_index].push_back(card.registryIndex);
			
			int column = 0;
			std::string cell;
			while(std::getline(ss, cell, ',' ))
			{
				switch( column )
				{
					case 0:
					{
						StringBuffer32 colour_string(cell.c_str());
						colour_string = colour_string.to_lower();
						card.colour = Colour::s_stringToType.at(colour_string.c_str());
						card.power[card.colour] = 1;
						break;
					}
					case 1:
					{
						card.points = atoi(cell.c_str());
						break;
					}
					case 2:
					case 3:
					case 4:
					case 5:
					case 6:
					{
						// i.e. white at column 0 is index 0
						int colour_index = column - 2;
						card.cost[colour_index] = atoi(cell.c_str());
						break;
					}
					// end
					default:
						break;
				}
				column++;
			}
		}
	}

	const Card* LookupCard(int index)
	{
		if(index < 0 || index >= s_cardRegistry.size())
			return nullptr;

		return &s_cardRegistry[index];
	}

	static void CreateCard(Card& card, int index)
	{
		if(index >= 0 && index < s_cardRegistry.size())
		{
			CopyComponent(card, s_cardRegistry[index]);
		
			// update the card base... based on the colour
			Sprite& sprite = GetComponentRef(Sprite, card.entity);

			StringBuffer64 coloured_sprite;
			AddColourPostfix(sprite.image.id.c_str(), card.colour, coloured_sprite);
			sprite.SetTexture(coloured_sprite.c_str());

			card.spell = SpellRegistry::GetSpell(card.points, card.colour);
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

	void ResetCards()
	{
		std::vector<Entity> entities;

		ComponentArray<Card>& cards = GetAllComponents(Card);
		for (auto iter = cards.entityToComponent.begin(); iter != cards.entityToComponent.end(); iter++)
		{
			const Card& card = cards.GetComponentByIndex(iter->second);
			entities.push_back(iter->first);
		}

		for (u32 i = 0; i < entities.size(); i++)
		{
			DiscardCard(entities[i]);
		}

		for (u32 i = 0; i < Card::c_tiers; i++)
		{
			s_cardRegistryDrawPile[i].clear();
			s_cardRegistryDiscard[i].clear();
		}

		std::vector<int> indexes;
		for (int i = 0; i < s_cardRegistry.size(); i++)
		{
			int tier = s_cardRegistry[i].tier;
			s_cardRegistryDrawPile[tier].push_back(i);
		}
	}
	
	void DiscardCard(Entity entity)
	{
		Card& card = GetComponentRef(Card, entity);
		
		// place into discard pile
		std::vector<int>& discard_pile = s_cardRegistryDiscard[card.tier];
		discard_pile.push_back(card.registryIndex);
		
		// remove the child displays
		DestroyChildren(entity);
		
		Sprite& sprite = GetComponentRef(Sprite, entity);
		sprite.params.disabled = true;

		// remove the component
		//RemoveComponent(Card, entity);
		card.registryIndex = -1;
	}

	void ReturnCardToDrawPile(ECS::Entity entity)
	{
		Card& card = GetComponentRef(Card, entity);
		
		// place into discard pile
		std::vector<int>& draw_pile = s_cardRegistryDrawPile[card.tier];
		draw_pile.push_back(card.registryIndex);
		
		// remove the child displays
		DestroyChildren(entity);
		
		Sprite& sprite = GetComponentRef(Sprite, entity);
		sprite.params.disabled = true;

		card.registryIndex = -1;
		// remove the component
		//RemoveComponent(Card, entity);
	}
	
	void DrawRandomCard(Entity entity, int tier)
	{
		std::vector<int>& draw_pile = s_cardRegistryDrawPile[tier];
		if(draw_pile.size() > 0)
		{
			int random_index = Maths::randomNumberBetween( 0, (int)draw_pile.size());
			int random_registry_index = draw_pile[random_index];

			DrawCard(entity, random_registry_index);
		}
	}

	void DrawCard(Entity entity, int index)
	{
		if(entity != EntityInvalid)
		{
			Card& new_card = GetOrAddComponent(Card, entity);
			CreateCard(new_card, index);

			Sprite& sprite = GetComponentRef(Sprite, entity);
			sprite.params.disabled = false;

			std::vector<int>& draw_pile = s_cardRegistryDrawPile[new_card.tier];
			for( auto iter = draw_pile.begin(); iter != draw_pile.end(); iter++ )
			{
				if(*iter == new_card.registryIndex)
				{
					draw_pile.erase(iter);
					break;
				}
			}

			TriggerGameEvent(GameEvent::CardDrawn, entity);
		}
	}

	void DrawCards()
	{
		ComponentArray<Card>& cards = GetAllComponents(Card);

		// [ entity, tier ] 
		// push these into a list first, otherwise we can invalidate the iterator
		std::vector< std::pair<Entity, int> > entities;
		for (auto iter = cards.entityToComponent.begin(); iter != cards.entityToComponent.end(); iter++)
		{
			const Card& card = cards.GetComponentByIndex(iter->second);
			entities.push_back({ iter->first, card.tier });
		}

		for (u32 i = 0; i < entities.size(); i++)
		{
			DrawRandomCard(entities[i].first, entities[i].second);
		}
	}
}