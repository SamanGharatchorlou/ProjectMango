#pragma once

namespace ECS
{
	struct Card;
	struct DeckCard;
}

namespace CardRegistry
{
	void ClearAll();
	void ReadomFromJson(const char* config, int tier);
	void ReadomFromCSV(const char* config, int tier);

	const ECS::Card* LookupCard(int index);
	ECS::Entity CreateCard(const char* id, VectorF world_pos, const ECS::DeckCard& dc);
	void PopulateDrawPiles(std::vector<ECS::DeckCard>* cards, int tier);
}