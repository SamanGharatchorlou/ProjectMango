#pragma once

namespace Scene
{
	ECS::Entity BuildBiome(const char* biome_id, int biome_index, int level_index);
	ECS::Entity BuildBiomeAndEntities(const char* biome_id, int biome_index, int level_index);
}