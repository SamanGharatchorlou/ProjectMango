#include "pch.h"
#include "PickupCallbacks.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpellComponents.h"
#include "ECS/Components/Components.h"

void ApplyReboundRune(ECS::Entity rune_entity, ECS::Entity hit_entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	if (ECS::SpellBook* sb = ecs->GetComponent(SpellBook, hit_entity))
	{
		if(ECS::Pickup* pick_up = ecs->GetComponent(Pickup, rune_entity))
		{
			ECS::Rune* rune = new ECS::ReboundRune(pick_up->config.c_str());
			sb->SetSpellSlotRune(0, rune);
		}
	}
}