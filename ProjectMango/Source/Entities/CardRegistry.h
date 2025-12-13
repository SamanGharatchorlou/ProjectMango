#pragma once

namespace ECS
{
	struct Card;
}

namespace CardRegistry
{
	void Build(const char* config, int tier);

	// invalid index == random
	void GetCard(ECS::Card& card, int index);
	//void GetRandomCard(ECS::Card& card, int tier);
	//void GetCard(ECS::Card& card, SColour colour);

	const ECS::Card* LookupCard(int index);

	int PickRandomIndex(int tier);

	void RemoveCard(ECS::Entity entity);
	void DrawCard( ECS::Entity entity, int index );
}