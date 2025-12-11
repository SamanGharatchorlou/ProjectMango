#pragma once

namespace ECS
{
	struct Card;
}

namespace CardRegistry
{
	void Build(const char* config);

	void GetRandomTier1Card(ECS::Card& card);

	void ReplaceCard(ECS::Card& card);
}