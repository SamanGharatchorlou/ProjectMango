#pragma once

namespace ECS
{
	struct EntityMetaData;
}

namespace SpellRegistry
{
	void Build(const char* config);
	ECS::Entity CreateSpell(const char* spell_id, ECS::Entity target);

	const char* GetSpell(int points, u32 colour);
	bool GetSpellMetaData(const char* spell_id, ECS::EntityMetaData& emd);
}