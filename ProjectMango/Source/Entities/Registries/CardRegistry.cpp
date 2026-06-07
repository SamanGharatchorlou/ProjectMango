#include "pch.h"
#include "CardRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "System/Files/JSONParser.h"
#include "System/Files/TextFileReader.h"
#include "ECS/EntityCoordinator.h"
#include "MonsterRegistry.h"
#include "System/Window.h"
#include "Entities/Factory/ComponentAssembler.h"

using namespace ECS;

namespace CardRegistry
{
	static std::vector<Card> s_cardRegistry;
	
	void ClearAll()
	{
		s_cardRegistry.clear();
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
					card.registryIndex = registry_index;
					card.tier = tier_index;
					card.damage = value["points"].GetInt();

					const Value::ConstArray& cost = value["cost"].GetArray();
					for( u32 c = 0; c < cost.Size(); c++ )
					{
						card.cost[c] = cost[c].GetInt();
					}
				}
			}
		}
	}
	
	void ReadomFromCSV(const char* file, int tier_index)
	{
		BasicString file_path;
		FileManager::Get()->FindFile( FileManager::Configs, file, file_path );
		TextFileReader reader(file_path.c_str());

		int row = 0;

		std::string line;
		while(std::getline(reader.mFile, line))
		{
			// read a line, each line is a card
			std::stringstream ss(line);

			// skip the headers
			if (row++ == 0)
				continue;

			s_cardRegistry.push_back( Card() );
			Card& card = s_cardRegistry.back();
			card.tier = tier_index;
			card.registryIndex = (int)s_cardRegistry.size() - 1;

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
						break;
					}
					case 1:
					{
						card.damage = atoi(cell.c_str());
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

			int total_cost = 0;
			for (u32 i = 0; i < Colour::Count; i++)
			{
				total_cost += card.cost[i];
			}

			if (total_cost == 0)
			{
				DebugPrint(Warning, "Card built with no cost - Colour:%s, Damage:%s", 
					Colour::s_typeToString.at(card.colour), card.damage);
			}
		}
	}

	const Card* LookupCard(int index)
	{
		if(index < 0 || index >= s_cardRegistry.size())
			return nullptr;

		return &s_cardRegistry[index];
	}

	static const VectorF s_cardSize = VectorF(24.0f, 32.0f);

	Entity CreateCard(const char* id, VectorF world_pos, const DeckCard& dc)
	{
		ECS::EntityMetaData emd;

		VectorF window_to_level = GameData::Get().window->windowToLevel;
		VectorF level_to_window;
		level_to_window.x = 1.0f / window_to_level.x;
		level_to_window.y = 1.0f / window_to_level.y;

		emd.data.AddString("id", id);
		emd.data.AddVectorF("size", s_cardSize * level_to_window);
		emd.data.AddVectorF("position", world_pos);
		emd.data.AddVectorF("pivot_point", VectorF(0.5f, 0.5f));
		emd.data.AddString("button_callback", "RequestCard");

		emd.data.AddString("sprite", "card_bases");
		emd.data.AddVectorF("sprite_sheet_frames", VectorF(Colour::Count, 1));
		if (const Card* registry_card = CardRegistry::LookupCard(dc.registryIndex))
		{
			emd.data.AddInt("colour_type", (int)registry_card->colour);
			emd.data.AddInt("card_registry_index", dc.registryIndex);

			emd.data.AddInt("sprite_sheet_index", registry_card->colour);
		}

		return AssembleEntity(emd);
	}

	void PopulateDrawPiles(std::vector<DeckCard>* cards, int tier)
	{
		for (int i = 0; i < s_cardRegistry.size(); i++)
		{
			int tier = s_cardRegistry[i].tier;

			DeckCard dc;
			dc.registryIndex = i;
			dc.tier = s_cardRegistry[i].tier;
			cards[tier].emplace_back(dc);
		}
	}
}