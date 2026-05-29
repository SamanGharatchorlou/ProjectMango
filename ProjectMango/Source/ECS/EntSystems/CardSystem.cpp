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
		if(Inventory* inventory = GetComponent(Inventory, player))
			inventory->GetBuyingPower(buying_power);

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

				for( int j = cost -1; j >= 0; --j )
				{
					Sprite& sprite = GetComponentRef(Sprite, cost_entities[j]);

					if(discounted > 0)
					{
						sprite.SetTexture("cost_discount");
						discounted--;
					}
					else if(b_power > 0)
					{
						sprite.SetTexture("cost_filled");
						b_power--;
					}
					else
					{
						sprite.SetTexture("cost_empty");
					}
				}
			}
		}
	}
}