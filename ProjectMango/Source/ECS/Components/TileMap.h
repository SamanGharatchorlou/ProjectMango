#pragma once

#include "Scene/SceneParsing/SceneBuilder.h"

namespace ECS
{
	struct TileMap
	{
		COMPONENT_TYPE(TileMap)

		Map::SceneTileMapping tileMap;

		static VectorF GetMapSizeRatio();
		static const TileMap* GetActive();
	};
}