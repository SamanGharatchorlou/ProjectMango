#pragma once

namespace ECS
{
	struct Card;
}

namespace CardRegistry
{
	void ClearAll();
	void ReadomFromJson(const char* config, int tier);
	void ReadomFromCSV(const char* config, int tier);

	const ECS::Card* LookupCard(int index);

	void ResetCards();
	void DiscardCard(ECS::Entity entity);
	void ReturnCardToDrawPile(ECS::Entity entity);

	void DrawCard( ECS::Entity entity, int index );
	void DrawRandomCard( ECS::Entity entity, int tier );

	// setup the board
	void DrawCards();
}