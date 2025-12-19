#pragma once

namespace ECS
{
	struct Card;
}

namespace CardRegistry
{
	void Build(const char* config, int tier);

	// invalid index == random
	//void GetCard(ECS::Card& card, int index);
	//void GetRandomCard(ECS::Card& card, int tier);
	//void GetCard(ECS::Card& card, SColour colour);

	const ECS::Card* LookupCard(int index);

	//int PickRandomIndex(int tier);

	void SetupDrawPile();
	void DiscardCard(ECS::Entity entity);
	void DrawCard( ECS::Entity entity, int index );
	void DrawRandomCard( ECS::Entity entity, int tier );
}