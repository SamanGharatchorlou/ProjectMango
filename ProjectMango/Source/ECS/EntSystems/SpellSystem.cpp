#include "pch.h"
#include "SpellSystem.h"

#include "ECS/EntityCoordinator.h"
#include "Entities/Spells/SpellEntityBuilder.h"
#include "ECS/Components/SpellComponents.h"
#include "ECS/Components/Animator.h"

namespace ECS
{
	void SpellSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		// setup spell - where to put this?
		Magic::CreateEntityMap();

		for (u32 i = 0; i < entitiesToDestroy.size(); i++)
		{
			ecs->entities.KillEntity(entitiesToDestroy[i]);
		}

		for (Entity entity : entities)
		{
			Spell& spell = ecs->GetComponentRef(Spell, entity);
			if (spell.rune)
			{
				spell.rune->Update(entity);
			}
			
			if( Animator* animator = ecs->GetComponent(Animator, entity) )
			{
				if(!animator->GetActiveAnimation().looping && animator->loopCount > 0)
				{
					entitiesToDestroy.push_back(entity);
				}
			}
		}
	}
}