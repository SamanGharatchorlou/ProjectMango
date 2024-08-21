#include "pch.h"
#include "LevelReader.h"

#include "System/Files/JSONParser.h"
#include "ECS/Components/Level.h"
#include "Graphics/TextureManager.h"
#include "System/Window.h"
#include "Core/Helpers.h"
#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"

namespace Level
{
	static std::unordered_map<int, ECS::TileSet> s_tileSets;
	
	using namespace rapidjson;
	
	static void LoadTileset(JSONParser& parser, int tileset_uid)
	{
		Value& defs = parser.document["defs"];
		Value& tilesets = defs["tilesets"];

		for( u32 i = 0; i < tilesets.Size(); i++ )
		{
			Value& json_tileset = tilesets[i];
			if( json_tileset["uid"] == tileset_uid )
			{
				ECS::TileSet tile_set;

				const char* tileset_name = json_tileset["relPath"].GetString();
				tile_set.texture = TextureManager::Get()->getTexture(tileset_name, FileManager::Maps);

				float sx = json_tileset["pxWid"].GetFloat();
				float sy = json_tileset["pxHei"].GetFloat();
				tile_set.mapSize = VectorF(sx, sy);
				
				float tw = json_tileset["tileGridSize"].GetFloat();
				tile_set.tileSize = VectorF(tw, tw);

				s_tileSets[tileset_uid] = tile_set;
				return;
			}
		}
	}

	void BuildLevel(ECS::Level& level, u32 level_number)
	{
		BasicString string = FileManager::Get()->findFile(FileManager::Maps, "test");
		JSONParser parser(string.c_str());

		Value& levels = parser.document["levels"];
		Value& numbered_level = levels[level_number - 1];

		int level_px_width = numbered_level["pxWid"].GetInt();
		int level_px_height = numbered_level["pxHei"].GetInt();
		
		int grid_length = parser.document["defaultGridSize"].GetInt();
		int level_width = level_px_width / grid_length;
		int level_height = level_px_height / grid_length;

		const VectorF window_size = GameData::Get().window->size();

		const float level_to_window_x = window_size.x / (float)level_px_width;
		const float level_to_window_y = window_size.y / (float)level_px_height;
		VectorF level_to_window(level_to_window_x, level_to_window_y);

		Value& layers = numbered_level["layerInstances"];
		for (SizeType i = 0; i < layers.Size(); i++)
		{
			Value& layer = layers[i];
			const char* layer_type = layer["__type"].GetString();
			if( StringCompare(layer_type, "Entities" ) )
			{
				const Value::Array& entities = layer["entityInstances"].GetArray();
				for( u32 e = 0; e < entities.Size(); e++ )
				{
					Value& entry = entities[e];
					if( StringCompare(entry["__identifier"].GetString(), "PlayerStart") )
					{
						float width = entry["width"].GetFloat();
						float height = entry["height"].GetFloat();

						Value& px = entry["px"];
						float px_x = px[0].GetFloat() + (width * 0.5f);
						float px_y = px[1].GetFloat() - (height);

						level.playerSpawn = VectorF(px_x, px_y) * level_to_window;
					}
				}
			}
			else if( StringCompare(layer_type, "IntGrid" ) )
			{
				const Value::Array& entities = layer["intGridCsv"].GetArray();

				struct GridBlock
				{
					VectorI top_left;
					VectorI bot_right;
					int value;
				};

				std::vector<GridBlock> blocks;


				for( u32 ent_x = 0; ent_x < level_width; ent_x++ )
				{
					for( u32 ent_y = 0; ent_y < level_height; ent_y++ )
					{
						int index = ent_y * level_width + ent_x;
						int value = entities[index].GetInt();

						if(value != 0)
						{
							// have we visited this index?
							bool visited = false;
							for( u32 b = 0; b < blocks.size(); b++ )
							{
								VectorI ent_idx(ent_x, ent_y);
							
								if(blocks[b].top_left.x <= ent_idx.x && blocks[b].top_left.y <= ent_idx.y)
								{
									if(blocks[b].bot_right.x >= ent_idx.x && blocks[b].bot_right.y >= ent_idx.y)
									{
										visited = true;
										break;
									}
								}
							}

							if(visited)
								continue;

							GridBlock block;
							block.top_left = VectorI(ent_x, ent_y);
							block.value = value;

							int x_idx = ent_x;
							while( x_idx < level_width )
							{
								int x_index = ent_y * level_width + x_idx;
								int x_value = entities[x_index].GetInt();

								if(x_value != value)
									break;

								x_idx++;
							}

							int y_idx = ent_y;
							while( y_idx < level_height )
							{
								bool exit = false;
								for( u32 xy_ent = ent_x; xy_ent < x_idx; xy_ent++ )
								{
									int y_index = y_idx * level_width + xy_ent;
									int y_value = entities[y_index].GetInt();

									if(y_value != value)
									{
										exit = true;
										break;
									}
								}

								if(exit)
									break;
								
								y_idx++;
							}

							block.bot_right = VectorI(x_idx - 1, y_idx - 1);
							blocks.push_back(block);
						}
					}
				}

				VectorF grid_size((float)grid_length, (float)grid_length);

				for( u32 b = 0; b < blocks.size(); b++ )
				{
					//if(blocks[b].value != 1)
					//	continue;

					VectorF top_left = blocks[b].top_left.toFloat() * grid_size * level_to_window;
					VectorF size = (blocks[b].bot_right + VectorI(1,1) - blocks[b].top_left).toFloat() * grid_size * level_to_window;

					RectF collider_rect(top_left, size);

					// create a static collider entity
					ECS::EntityCoordinator* ecs = GameData::Get().ecs;
					ECS::Entity ent = ecs->CreateNewEntity();

					ECS::Collider& collider = ecs->AddComponent(Collider, ent);
					collider.SetRect( collider_rect );
					SetFlag<u32>(collider.mFlags, (u32)ECS::Collider::Static);

					ECS::Transform& transform = ecs->AddComponent(Transform, ent);
					transform.SetPosition(top_left);
				}
			}
			else
			{
				ECS::Layer new_layer;
				level.layers.push_back(new_layer);
				ECS::Layer& level_layer = level.layers.back();
			
				const char* tileset_name = layer["__tilesetRelPath"].GetString();
				int tileset_uid = layer["__tilesetDefUid"].GetInt();

				if(!s_tileSets.contains(tileset_uid))
					LoadTileset(parser, tileset_uid);

				ECS::TileSet& tile_set = s_tileSets.at(tileset_uid);
				level_layer.tileSet = &tile_set;
				level_layer.tileSize = tile_set.tileSize * level_to_window;

				const Value::Array& grid_array = layer["gridTiles"].GetArray();
				for( u32 i = 0; i < grid_array.Size(); i++ )
				{
					Value& entry = grid_array[i];

					Value& px = entry["px"];
					float px_x = px[0].GetFloat();
					float px_y = px[1].GetFloat();
					 
					Value& src = entry["src"];
					float src_x = src[0].GetFloat();
					float src_y = src[1].GetFloat();

					ECS::Layer::Tile tile;
					tile.draw_pos = VectorF(px_x, px_y) * level_to_window;
					tile.tileset_pos = VectorF(src_x, src_y);
					level_layer.tiles.push_back(tile);
				}
			}
		}
	}
}