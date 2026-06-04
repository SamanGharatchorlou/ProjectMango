#include "pch.h"
#include "StatusEffectRegistry.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Objects/CardBoard.h"

using namespace ECS;

namespace StatusEffectRegistry
{
	typedef bool(*OnApplyEffectFn)(Entity entity);

	std::unordered_map<BasicString, OnApplyEffectFn> s_onApplyEffectsRegistry;
	
	static bool DestroyRandomBoardCard(Entity entity)
	{
		ComponentArray<Card>& cards =  GetAllComponents(Card);

		std::vector<Card*> valid_cards;
		for (auto& [key, value] : cards.entityToComponent)
		{
			Card& card = cards.GetComponentByIndex(value);
			if (card.registryIndex != -1)
			{
				valid_cards.push_back(&card);
			}
		}

		if (valid_cards.size() > 0)
		{
			int index = Maths::randomNumberBetween(0, (int)valid_cards.size());
			const Card* card = valid_cards[index];
			DiscardCard(card->entity);
		}

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