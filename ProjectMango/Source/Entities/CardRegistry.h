#pragma once

namespace ECS
{
	struct Card;
}

namespace CardRegistry
{
	void Build(const char* config, int tier);

	const ECS::Card* LookupCard(int index);

	void DiscardCard(ECS::Entity entity);
	void DrawCard( ECS::Entity entity, int index );
	void DrawRandomCard( ECS::Entity entity, int tier );
}