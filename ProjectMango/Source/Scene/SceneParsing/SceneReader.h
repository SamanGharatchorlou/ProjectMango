#pragma once

namespace ECS
{
	struct Biome;
}

namespace Scene
{
	void BuildBiome(const char* biome_id, ECS::Biome& biome);
}