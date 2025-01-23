#include "pch.h"
#include "PickupCallbacks.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpellComponents.h"

void ApplyReboundRune(ECS::Entity rune_entity, ECS::Entity hit_entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	if (ECS::SpellBook* sb = ecs->GetComponent(SpellBook, hit_entity))
	{
		ECS::Rune* rune = new ECS::ReboundRune;
		sb->SetSpellSlotRune(0, rune);
	}
}