#pragma once

namespace ECS
{
	struct Level;

	bool IsInLevel(const ECS::Level& level, const Transform& transform);
	void GetEntitiesInLevel(const Level& level, std::unordered_map<Entity, u32>& in_entities, std::vector<Entity>& out_entities);

}