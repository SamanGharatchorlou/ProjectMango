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
	void GetSpellMetaData(const char* spell_id, ECS::EntityMetaData& emd);

	//const char* GetMonster(int index);
	//const char* GetRandomMonster(int points);
	//int GetRandomMonsterIndex(int points);
}