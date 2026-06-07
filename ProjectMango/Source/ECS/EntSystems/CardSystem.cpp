#include "pch.h"
#include "CardSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/IncludeComponents.h"
#include "Core/Helpers.h"

namespace ECS
{
	void CardSystem::Update(float dt)
	{
		UICursor* cursor = UICursor::Get();

		int buying_power[Colour::Count] = { 0 };

		Entity player = Faction::GetPlayer();
		const Inventory& inventory = GetComponentRef(Inventory, player);
		inventory.GetBuyingPower(buying_power);

		// increase buying power by 1 if we have this relic
		int relic_reductions[Colour::Count] = { 0 };
		if (inventory.OwnsRelic("LifeForCost_Black"))
		{
			relic_reductions[Colour::Black] += 2;
		}

		for (Entity entity : entities)
		{
			// debug break point
			if(IsSelectedDebugEntity(entity))
				int a = 4;

			Card& card = GetComponentRef(Card, entity);
			if (card.registryIndex == -1)
				continue;

			// fill in affordable icons
			for( int i = 0; i < Colour::Count; i++ )
			{
				if(card.cost[i] == 0)
					continue;

				std::vector<Entity>& cost_entities = card.costEntities[i];

				int cost = (int)cost_entities.size();
				int b_power = buying_power[i];
				int discounted = card.discount[i];
				int relic_reduction = relic_reductions[i];

				for( int j = cost -1; j >= 0; --j )
				{
					Sprite& sprite = GetComponentRef(Sprite, cost_entities[j]);
					sprite.params.colourMod = Colour::s_typeToColour.at((Colour::Type)i);

					// discount cards first
					if(discounted > 0)
					{
						sprite.SetTexture("cost_discount");
						discounted--;
					}
					// then we would use player coins
					else if(b_power > 0)
					{
						sprite.SetTexture("cost_filled");
						b_power--;
					}
					// then consume life
					else if (relic_reduction > 0)
					{
						sprite.SetTexture("cost_heart");
						sprite.params.colourMod = SColour();
						relic_reduction--;
					}
					// otherwise we cant afford and its empty
					else
					{
						sprite.SetTexture("cost_empty");
					}
				}
			}
		}
	}
}