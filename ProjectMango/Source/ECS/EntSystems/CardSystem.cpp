#include "pch.h"
#include "CardSystem.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/GameComponents.h"
#include "ECS/Components/UIComponents.h"
#include "Core/Helpers.h"

namespace ECS
{
	void CardSystem::Update(float dt)
	{

		UICursor* cursor = UICursor::Get();
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
		}
	}
}