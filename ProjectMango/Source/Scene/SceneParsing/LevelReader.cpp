#include "pch.h"
#include "LevelReader.h"

#include "System/Files/JSONParser.h"
#include "ECS/Components/Level.h"
#include "Graphics/TextureManager.h"
#include "System/Window.h"

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

		float level_width = numbered_level["pxWid"].GetFloat();
		float level_height = numbered_level["pxHei"].GetFloat();

		const VectorF window_size = GameData::Get().window->size();

		const float level_to_window_x = window_size.x / level_width;
		const float level_to_window_y = window_size.y / level_height;
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