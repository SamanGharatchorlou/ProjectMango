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

		int buying_power[Colour::Count];

		Entity player = Faction::GetPlayer();
		if(Inventory* inventory = GetComponent(Inventory, player))
			inventory->GetBuyingPower(buying_power);

		for (Entity entity : entities)
		{
			Card& card = GetComponentRef(Card, entity);
			Entity monster_entity = card.GetMonster();
			if(monster_entity != EntityInvalid)
			{
				EntityState& state = GetComponentRef(EntityState, monster_entity);
				if(cursor)
				{
					Transform& transform = GetComponentRef(Transform, entity);
					if(Contains(transform.GetRect(), cursor->Position()))
					{
						state.next = Action::Idle;
						continue;
					}
				}

				state.next = Action::Inactive;
			}
								
			for( int i = 0; i < Colour::Count; i++ )
			{
				std::vector<Entity>& cost_entities = card.costEntities[i];
				for( int j = 0; j < cost_entities.size(); j++ )
				{
					Sprite& sprite = GetComponentRef(Sprite, cost_entities[j]);
					sprite.SetTexture("cost_empty");
				}
			}

			for( int i = 0; i < Colour::Count; i++ )
			{
				if(card.cost[i] == 0)
					continue;

				std::vector<Entity>& cost_entities = card.costEntities[i];
				for( int j = 0; j < buying_power[i]; j++ )
				{
					if(cost_entities.size() > j)
					{
						Sprite& sprite = GetComponentRef(Sprite, cost_entities[j]);
						sprite.SetTexture("cost_filled");
					}
				}
			}

			for( int i = 0; i < Colour::Count; i++ )
			{
				if(card.discount[i] == 0)
					continue;

				std::vector<Entity>& cost_entities = card.costEntities[i];
				
				for( u32 j = 0; j < card.discount[i]; j++ )
				{
					int index = cost_entities.size() - 1;
					index = index - j;
					
					if(cost_entities.size() > index)
					{
						Sprite& sprite = GetComponentRef(Sprite, cost_entities[index]);
						sprite.SetTexture("cost_removed");
					}
				}
				
				//for( int j = 0; j < buying_power[i]; j++ )
				//{
				//	if(cost_entities.size() > j)
				//	{
				//		Sprite& sprite = GetComponentRef(Sprite, cost_entities[j]);
				//		sprite.SetTexture("cost_filled");
				//	}
				//}
			}
		}
	}
}