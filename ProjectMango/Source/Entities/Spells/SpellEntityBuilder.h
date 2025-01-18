#pragma once

namespace Magic
{
	void CreateEntityMap();
	ECS::Entity GetNewEntity(const char* id, ECS::Entity caster, VectorF target);

	bool SpellExists(const char* spell_name);
}