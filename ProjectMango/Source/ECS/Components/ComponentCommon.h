#pragma once

namespace ECS
{
	struct Level;
	struct Transform;

	bool IsInLevel(const Level& level, const Transform& transform);
	void GetEntitiesInLevel(const Level& level, const std::unordered_map<Entity, u32>& in_entities, std::vector<Entity>& out_entities);
}