#include "pch.h"
#include "PickupCallbacks.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/SpellComponents.h"
#include "ECS/Components/Components.h"

namespace PickUps
{
	std::unordered_map<BasicString, OnPickupFn> s_onPickupFunctions;

	static bool ReboundRune(ECS::Entity rune_entity, ECS::Entity hit_entity)
	{
		if (ECS::SpellBook* sb = GetComponent(SpellBook, hit_entity))
		{
			if (ECS::Pickup* pick_up = GetComponent(Pickup, rune_entity))
			{
				ECS::ReboundRune* rune = new ECS::ReboundRune(pick_up->itemId.c_str(), pick_up->config.c_str());
				sb->SetSpellSlotRune(0, rune);
				return true;
			}
		}

		return false;
	}

	bool EchoRune(ECS::Entity rune_entity, ECS::Entity hit_entity)
	{
		if (ECS::SpellBook* sb = GetComponent(SpellBook, hit_entity))
		{
			if (ECS::Pickup* pick_up = GetComponent(Pickup, rune_entity))
			{
				ECS::EchoRune* rune = new ECS::EchoRune(pick_up->itemId.c_str(), pick_up->config.c_str());
				sb->SetSpellSlotRune(0, rune);
				return true;
			}
		}

		return false;
	}

	void SetCallbacks()
	{
		// runes
		std::unordered_map<BasicString, OnPickupFn> OnPickupFunctions;
		s_onPickupFunctions["ReboundRune"] = ReboundRune;
		s_onPickupFunctions["EchoRune"] = EchoRune;
	}

	OnPickupFn GetCallback(const char* pickup_item)
	{
		if (pickup_item)
		{
			if (s_onPickupFunctions.contains(pickup_item))
				return s_onPickupFunctions[pickup_item];
		}

		return nullptr;
	}
}