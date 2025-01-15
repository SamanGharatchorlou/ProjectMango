#pragma once

namespace Spell
{
	void CreateEntityMap();
	ECS::Entity GetNewEntity(const char* id, ECS::Entity caster, VectorF target);
}