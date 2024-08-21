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
		COMPONENT_TYPE(Level)

		VectorF worldPos;
		VectorF size;

		std::vector<Layer> layers;

		std::vector<ECS::Entity> colliders;

		VectorF playerSpawn;

		static const Entity GetActive();
	};
}