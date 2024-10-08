#include "pch.h"
#include "TileMapSystem.h"

#include "ECS/Components/Components.h"
#include "ECS/Components/Biome.h"
#include "ECS/EntityCoordinator.h"
#include "Game/Camera/Camera.h"
#include "Graphics/RenderManager.h"

namespace ECS
{
	void TileMapSystem::Update(float dt)
	{
		EntityCoordinator* ecs = GameData::Get().ecs;
		RenderManager* rm = GameData::Get().renderManager;
		
		// increase the camera size so we draw a little extra than the actual screen
		// if we're moving (especially fast) we might get white bars where since the 
		// order of move the camera/player and these updates can be anything
		RectF camera_rect = Camera::Get()->GetRect();
		VectorF center = camera_rect.Center();
		camera_rect.SetSize(camera_rect.Size() * 1.1f);
		camera_rect.SetCenter(center);

		for (Entity entity : entities)
		{
			const Biome& biome = ecs->GetComponentRef(Biome, entity);

			for( u32 l = 0; l < biome.levels.size(); l++ )
			{
				const Level& level = biome.levels[l];

				for( int i = (int)level.layers.size() - 1; i >= 0; i-- )
				{
					const Layer& layer = level.layers[i];
					const VectorF& tile_size = layer.tileSet->tileSize;
					const Texture* tile_texture = layer.tileSet->texture;

					for( u32 j = 0; j < layer.tiles.size(); j++ )
					{
						RectF rect(layer.tiles[j].draw_pos, layer.tileSize);

						if(!camera_rect.Intersect(rect))
							continue;

						RectF subRect(layer.tiles[j].tileset_pos, tile_size);

						RenderPack pack(tile_texture, rect, 1);
						pack.subRect = subRect;

						rm->AddRenderPacket(pack);
					}
				}
			}
		}
	}
}