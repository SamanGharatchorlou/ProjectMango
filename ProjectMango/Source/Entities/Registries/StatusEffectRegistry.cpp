#include "pch.h"
#include "StatusEffectRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Registries/CardRegistry.h"

using namespace ECS;

namespace StatusEffectRegistry
{
	typedef bool(*OnApplyEffectFn)(Entity entity);

	std::unordered_map<BasicString, OnApplyEffectFn> s_onApplyEffectsRegistry;
	
	static bool DestroyRandomBoardCard(Entity entity)
	{
		ComponentArray<Card>& cards =  GetAllComponents(Card);
		int index = Maths::randomNumberBetween(0, cards.Count());
		
		const Card& card = cards.GetComponentByIndex(index);
		ASSERT( ecs->IsAlive(card.entity), "Trying to remove card that does not exist, cannot get component by index like this?");

		// i need a debug thing to show me how many parts exist
		CardRegistry::DiscardCard(card.entity);

		bool finished = true;
		return finished;
	}

	void PopulateRegistry()
	{
		s_onApplyEffectsRegistry.clear();

		s_onApplyEffectsRegistry.insert( { "DestroyRandomBoardCard", DestroyRandomBoardCard});
	} 
	
	void OnApply(ECS::StatusEffect& effect, Entity target)
	{
		s_onApplyEffectsRegistry.at(effect.type)(target);
	}
}