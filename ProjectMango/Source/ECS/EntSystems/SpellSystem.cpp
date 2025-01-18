#include "pch.h"
#include "SpellSystem.h"

#include "ECS/EntityCoordinator.h"
#include "Entities/Spells/SpellEntityBuilder.h"
#include "ECS/Components/SpellComponents.h"

namespace ECS
{
	void SpellSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		// setup spell - where to put this?
		Magic::CreateEntityMap();

		for (Entity entity : entities)
		{
			Spell& spell = ecs->GetComponentRef(Spell, entity);
			if (spell.gem)
			{
				spell.gem->Update(entity);
			}
		}
	}
}