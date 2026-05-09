#include "pch.h"
#include "StatusEffectRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"

#include "Entities/Registries/CardRegistry.h"

using namespace ECS;

namespace StatusEffectRegistry
{
	typedef bool(*OnApplyEffectFn)(const StatusEffect& effect, Entity entity);

	std::unordered_map<BasicString, OnApplyEffectFn> s_effectsRegistry;
	
	static bool DestroyBoardCard(const StatusEffect& relic, ECS::Entity entity)
	{
		// to start with just remove the card component from one of the cards
		// i need a debug thing to show me how many parts exist
		CardRegistry::DiscardCard(entity);

		bool finished = true;
		return finished;
	}

	void PopulateRegistry()
	{
		s_effectsRegistry.clear();

		s_effectsRegistry.insert( { BasicString("DestroyCard"), DestroyBoardCard});
		//effect_1.type = StatusEffect::DisableBoardCard;


	} 
}