#pragma once

namespace ECS
{
	//VectorF GetPosition(Entity entity);

	//Entity GetParent(Entity child);

	struct Level;

	void FilterEntitiesInLevel(const ECS::Level& level, const std::vector<ECS::Entity>& in_entities, std::vector<ECS::Entity>& out_entities);
}