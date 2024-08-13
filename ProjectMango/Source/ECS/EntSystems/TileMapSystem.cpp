#include "pch.h"
#include "TileMapSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/TileMap.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "System/Window.h"

namespace ECS
{
	using namespace Map;

	void TileMapSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;

		for (Entity entity : entities)
		{
			const TileMap& tile_map = ecs->GetComponentRef(TileMap, entity);

			const VectorF window_size = GameData::Get().window->size();
			const VectorF map_size = tile_map.tileMap.mapSize;
			const VectorF size_ratio = window_size / map_size;
			
			const SceneTileMapping& map = tile_map.tileMap;
			const VectorF tile_size = map.tileSize * size_ratio;

			for (u32 l = 0; l < tile_map.tileMap.tileLayers.size(); l++)
			{
				const TileLayer& layer = tile_map.tileMap.tileLayers[l];

				for (u32 idx = 0; idx < layer.ids.size(); idx++)
				{
					const u32 index = layer.ids[idx];
					if (index == 0) // change to constexpr and give name
						continue;
					
					// find the correct tileindex given the layer index and starting tile indexes
					for (u32 i = 0; i < map.tilesets.size(); i++)
					{
						const TileSet& ts = map.tilesets[i];
						if (index >= ts.startingIndex)
						{
							// Rect for on screen size and position
							VectorI map_index = IndexToMapIndex(idx, map.tileCount);
							VectorF pos = map_index.toFloat() * tile_size;
							RectF rect(pos, tile_size);

							const u32 local_tileset_index = index - ts.startingIndex;
							VectorI tileset_index = IndexToMapIndex(local_tileset_index, ts.tileCount.toInt());

							// SubRect for tile map entry so we know what bit of texture to draw
							VectorF top_left = tileset_index.toFloat() * ts.tileSize;
							RectF subRect(top_left, ts.tileSize);

							RenderPack pack(ts.texture, rect, 5);
							pack.subRect = subRect;

							GameData::Get().renderManager->AddRenderPacket(pack);
							break;
						}
					}
				}
			}
		}
	}
}