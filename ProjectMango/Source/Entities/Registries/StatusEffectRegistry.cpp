#include "pch.h"
#include "StatusEffectRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

using namespace ECS;

namespace StatusEffectRegistry
{
	typedef void(*OnApplyEffectFn)(const StatusEffect& effect, Entity entity);

	std::unordered_map<StatusEffect::Type, OnApplyEffectFn> s_effectsRegistry;
	
	static void DisableBoardCard(const StatusEffect& relic, ECS::Entity entity)
	{
		// to start with just remove the card component from one of the cards
		// i need a debug thing to show me how many parts exist
	}

	void PopulateRegistry()
	{
		s_effectsRegistry.clear();

		s_effectsRegistry.insert( { StatusEffect::DisableBoardCard, DisableBoardCard } );
		//effect_1.type = StatusEffect::DisableBoardCard;

	}
}