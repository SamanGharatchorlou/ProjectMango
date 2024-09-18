#pragma once

namespace ECS
{
	struct Biome;
}

namespace Level
{
	void BuildBiome(const char* biome_id, ECS::Biome& biome);
}