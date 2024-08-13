#pragma once

class Texture;

namespace Map
{
	struct TileSet
	{
		Texture* texture;

		VectorF mapSize;
		VectorF tileSize;
		VectorI tileCount;

		u32 startingIndex;
	};

	struct TileLayer
	{
		u32 renderLayer;
		VectorI tileCount;
		StringBuffer32 name;
		std::vector<u32> ids;
	};

	struct ObjectLayer
	{
		u32 id;
		StringBuffer32 name;
		std::vector<RectF> rects;
	};

	struct SceneTileMapping
	{
		std::vector<TileSet> tilesets;
		std::vector<TileLayer> tileLayers;

		std::vector<ObjectLayer> objectLayers;
		std::vector<ECS::Entity> colliderEntities;

		RectF playerSpawnArea;
		RectF enemySpawnArea;

		VectorF mapSize;
		VectorF tileSize;
		VectorI tileCount;
	};

	static VectorI IndexToMapIndex(u32 index, Vector2D<int> size)
	{
		int row = index / size.x;
		int column = index % size.x;
		return VectorI(column, row);
	}

	namespace SceneBuilder
	{
		void BuildTileMap(const char* mapName, SceneTileMapping& map_layers);
	};
}