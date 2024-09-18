#pragma once

class Texture;

namespace ECS
{
	struct TileSet
	{
		Texture* texture;

		VectorF mapSize;
		VectorF tileSize;
	};

	struct Layer
	{
		struct Tile
		{
			VectorF draw_pos;
			VectorF tileset_pos;
		};

		std::vector<Tile> tiles;
		VectorF tileSize;

		TileSet* tileSet;
	};

	struct Level
	{
		VectorF worldPos;
		VectorF size;

		u32 index;

		std::vector<Layer> layers;
		std::vector<ECS::Entity> colliders;
		std::unordered_map<StringBuffer64, VectorF> spawnPositions;
	};

	struct Biome
	{
		COMPONENT_TYPE(Level)

		std::vector<Level> levels;

		const Level& GetLevel(ECS::Entity entity) const;
		const Level& GetLevel(VectorF position) const;
		const Level& GetVisibleLevel() const;

		static const Entity GetActive();
		static bool GetSpawnPos(const char* spawn_id, VectorF& out_pos);
	};
}