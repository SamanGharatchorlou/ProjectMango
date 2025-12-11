#include "pch.h"

#include "CardRegistry.h"
#include "ECS/Components/Components.h"
#include "System/Files/JSONParser.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/UIEntityBuilder.h"
#include "Graphics/TextureManager.h"

namespace CardRegistry
{
	struct CardTierGroup
	{
		std::vector<ECS::Card> cards;
	};

	CardTierGroup s_cardRegistry[ECS::Card::c_tiers];

	void Build(const char* file)
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

		//parser.Print();

		if(parser.document.HasMember("Tier1Cards"))
		{
			if(parser.document["Tier1Cards"].IsArray())
			{
				const Value::Array& t1_cards = parser.document["Tier1Cards"].GetArray();

				CardTierGroup& tier_1 = s_cardRegistry[0];
				tier_1.cards.resize(t1_cards.Size());

				for( u32 i = 0; i < t1_cards.Size(); i++ )
				{
					const Value& value = t1_cards[i];
					ECS::Card& card = tier_1.cards[i];

					StringBuffer32 label = value["colour"].GetString();
					card.colour = ECS::Coin::s_stringToType.at( label );
					card.power[card.colour] = 1;

					const Value::ConstArray& cost = value["cost"].GetArray();
					for( u32 c = 0; c < cost.Size(); c++ )
					{
						card.cost[c] = cost[c].GetInt();
					}
				}
			}
		}
	}

	void GetRandomTier1Card(ECS::Card& card)
	{
		// we need to keep this, wipe everything else
		ECS::Entity entity = card.entity;

		CardTierGroup& gp = s_cardRegistry[0];
		int random_index = Maths::randomNumberBetween( 0, (int)gp.cards.size());
		card = gp.cards[random_index];
		card.entity = entity;
		
		// update the colour
		ECS::Sprite& sprite = GetComponentRef(Sprite, card.entity);
		sprite.colourMod = SColour( ECS::Coin::s_typeToColour.at(card.colour) );
	}

	
	void ReplaceCard(ECS::Card& card)
	{	
		RecreateCardFromCard(card.entity);

		// destroy the old card (and its children)
		ecs->entities.KillEntity(card.entity);
	}
}