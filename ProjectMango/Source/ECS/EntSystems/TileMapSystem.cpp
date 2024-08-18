#include "pch.h"
#include "TileMapSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/EntityCoordinator.h"
#include "Graphics/RenderManager.h"
#include "ECS/Components/Level.h"

namespace ECS
{
	void TileMapSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		RenderManager* rm = GameData::Get().renderManager;

		for (Entity entity : entities)
		{
			const Level& level = ecs->GetComponentRef(Level, entity);

			for( int i = (int)level.layers.size() - 1; i >= 0; i-- )
			{
				const Layer& layer = level.layers[i];
				const VectorF& tile_size = layer.tileSet->tileSize;
				const Texture* tile_texture = layer.tileSet->texture;

				for( u32 j = 0; j < layer.tiles.size(); j++ )
				{
					RectF rect(layer.tiles[j].draw_pos, layer.tileSize);
					RectF subRect(layer.tiles[j].tileset_pos, tile_size);

					RenderPack pack(tile_texture, rect, 5);
					pack.subRect = subRect;

					rm->AddRenderPacket(pack);
				}
			}
		}
	}
}