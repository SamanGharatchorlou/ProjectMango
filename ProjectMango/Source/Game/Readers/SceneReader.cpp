#include "pch.h"
#include "SceneReader.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Factory/ComponentAssembler.h"
#include "Graphics/TextureManager.h"
#include "System/Files/JSONParser.h"
#include "System/Window.h"
#include "UI/UIManager.h"

namespace Scene
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
		
	void ParseUILayer(Value& ui_layer)
	{
		const char* id = ui_layer["identifier"].GetString();
		UIScreenMetaData& screen_meta_data = UIManager::Get().screenMetaData[BasicString(id)];

		Value& layers = ui_layer["layerInstances"];
		for (SizeType i = 0; i < layers.Size(); i++)
		{
			const Value::Array& entities = layers[i]["entityInstances"].GetArray();
			for (u32 e = 0; e < entities.Size(); e++)
			{
				Value& entry = entities[e];

				ECS::EntityMetaData emd;
				PopulateMetaData(entry, emd);

				screen_meta_data.push_back(emd);
			}
		}
	}

	struct Params
	{
		ECS::Level* level = nullptr;

		std::unordered_map<int, BasicString> valueDefines;

		VectorF levelToWindow;
		VectorF levelSize;
		int gridLength = 0;
	};

	static void ParseEntitiesLayer(Value& layer, ECS::Level& level)
	{
		const Value::Array& entities = layer["entityInstances"].GetArray();
		for (u32 e = 0; e < entities.Size(); e++)
		{
			Value& entry = entities[e];

			ECS::EntityMetaData emd;
			PopulateMetaData(entry, emd);

			level.entityMetaData.push_back(emd);
		}
	}

	static void ParseTerrainColliders(Value& layer, const Params& params)
	{
		const Value::Array& entities = layer["intGridCsv"].GetArray();

		struct GridBlock
		{
			VectorI top_left;
			VectorI bot_right;
			int value = 0;
		};

		std::vector<GridBlock> blocks;

		for (u32 ent_x = 0; ent_x < params.levelSize.x; ent_x++)
		{
			for (u32 ent_y = 0; ent_y < params.levelSize.y; ent_y++)
			{
				int index = ent_y * params.levelSize.x + ent_x;
				int value = entities[index].GetInt();

				if (value != 0)
				{
					// have we visited this index?
					bool visited = false;
					for (u32 b = 0; b < blocks.size(); b++)
					{
						VectorI ent_idx(ent_x, ent_y);

						if (blocks[b].top_left.x <= ent_idx.x && blocks[b].top_left.y <= ent_idx.y)
						{
							if (blocks[b].bot_right.x >= ent_idx.x && blocks[b].bot_right.y >= ent_idx.y)
							{
								visited = true;
								break;
							}
						}
					}

					if (visited)
						continue;

					GridBlock block;
					block.top_left = VectorI(ent_x, ent_y);
					block.value = value;

					int x_idx = ent_x;
					while (x_idx < params.levelSize.x)
					{
						int x_index = ent_y * params.levelSize.x + x_idx;
						int x_value = entities[x_index].GetInt();

						if (x_value != value)
							break;

						x_idx++;
					}

					int y_idx = ent_y;
					while (y_idx < params.levelSize.y)
					{
						bool exit = false;
						for (u32 xy_ent = ent_x; xy_ent < x_idx; xy_ent++)
						{
							int y_index = y_idx * params.levelSize.x + xy_ent;
							int y_value = entities[y_index].GetInt();

							if (y_value != value)
							{
								exit = true;
								break;
							}
						}

						if (exit)
							break;

						y_idx++;
					}

					block.bot_right = VectorI(x_idx - 1, y_idx - 1);
					blocks.push_back(block);
				}
			}
		}

		const VectorF grid_size = VectorF((float)params.gridLength) * params.levelToWindow;
		for (u32 b = 0; b < blocks.size(); b++)
		{
			const GridBlock& block = blocks[b];

			VectorF top_left = block.top_left.toFloat() * grid_size;
			VectorI block_size = block.bot_right + VectorI(1, 1) - block.top_left;
			VectorF size = block_size.toFloat() * grid_size;

			RectF collider_rect(top_left, size);

			// create a static collider entity
			char buffer[32];
			snprintf(buffer, 32, "Map Collider %d", (int)b);
			ECS::Entity ent = ECS::CreateEntity(buffer);

			AddComponent(Transform, ent);
			AddComponent(Collider, ent);

			ECS::Transform& transform = GetComponentRef(Transform, ent);
			ECS::Collider& collider = GetComponentRef(Collider, ent);
			transform.size = size;
			transform.SetWorldPosition(top_left);

			collider.Init();
			collider.SetFlag(ECS::Collider::Static);
			collider.SetFlag(ECS::Collider::IsTerrain);
			collider.SetFlag(ECS::Collider::IgnoreDamage);

			if (params.valueDefines.contains(block.value))
			{
				const char* type = params.valueDefines.at(block.value).c_str();
				if (StringCompare(type, "Floor"))
				{
					collider.SetFlag(ECS::Collider::IsFloor);
				}
				else if (StringCompare(type, "Wall"))
				{
					collider.SetFlag(ECS::Collider::IsWall);
				}
			}
		}

		params.level->walkableTiles = Grid<int>(params.levelSize.x, params.levelSize.y, 0);
		for (u32 ent_x = 0; ent_x < params.levelSize.x; ent_x++)
		{
			for (u32 ent_y = 0; ent_y < params.levelSize.y; ent_y++)
			{
				int index = ent_y * params.levelSize.x + ent_x;
				int value = entities[index].GetInt();
				params.level->walkableTiles.get(ent_x, ent_y) = value;
			}
		}
	}

	static void ParseLayerBoundaries(Value& layer, JSONParser& parser, const Params& params)
	{
		ECS::Layer new_layer;
		params.level->layers.push_back(new_layer);
		ECS::Layer& level_layer = params.level->layers.back();

		const char* tileset_name = layer["__tilesetRelPath"].GetString();
		int tileset_uid = layer["__tilesetDefUid"].GetInt();

		if (!s_tileSets.contains(tileset_uid))
			LoadTileset(parser, tileset_uid);

		ECS::TileSet& tile_set = s_tileSets.at(tileset_uid);
		level_layer.tileSet = &tile_set;
		level_layer.tileSize = tile_set.tileSize * params.levelToWindow;

		const Value::Array& grid_array = layer["gridTiles"].GetArray();
		for (u32 i = 0; i < grid_array.Size(); i++)
		{
			Value& entry = grid_array[i];

			Value& px = entry["px"];
			float px_x = px[0].GetFloat() * params.levelToWindow.x;
			float px_y = px[1].GetFloat() * params.levelToWindow.y;

			VectorF tile_pos = VectorF(px_x, px_y) + params.level->worldPos;

			Value& src = entry["src"];
			float src_x = src[0].GetFloat();
			float src_y = src[1].GetFloat();

			ECS::Layer::Tile tile;
			tile.draw_pos = tile_pos;
			tile.tileset_pos = VectorF(src_x, src_y);
			level_layer.tiles.push_back(tile);
		}
	}

	void BuildBiome(const char* biome_id, ECS::Entity& biome_entity)
	{
		BasicString file;
		FileManager::Get()->FindFile(FileManager::Maps, biome_id, file);

		JSONParser parser(file.c_str());
		if(!parser.IsValid())
			return;

		ECS::Biome& biome = GetComponentRef(Biome, biome_entity);

		const VectorF window_size = GameData::Get().window->size();

		// base screen level width is 256, i.e. thats the non-streched size of the screen
		const float level_to_window_x = window_size.x / parser.document["defaultLevelWidth"].GetFloat();;
		const float level_to_window_y = window_size.y / parser.document["defaultLevelHeight"].GetFloat();
		VectorF level_to_window(level_to_window_x, level_to_window_y);
		GameData::Get().window->windowToLevel = VectorF( 1.0f / level_to_window_x, 1.0f / level_to_window_y);

		Params params;
		params.gridLength = parser.document["defaultGridSize"].GetInt();
		params.levelToWindow = level_to_window;

		Value& defines = parser.document["defs"];
		Value::Array layers = defines["layers"].GetArray();
		for( u32 i = 0; i < layers.Size(); i++ )
		{
			if( StringCompare( layers[i]["identifier"].GetString(), "TerrainColliders" ) )
			{
				const Value::Array& grid_values = layers[i]["intGridValues"].GetArray();
				for( u32 i = 0; i < grid_values.Size(); i++ )
				{
					Value& grid_value = grid_values[i];
					params.valueDefines[grid_value["value"].GetInt()] = grid_value["identifier"].GetString();
				}
			}
		}

		u32 level_index = 0;

		const Value::Array& levels = parser.document["levels"].GetArray();
		for( u32 i = 0; i < levels.Size(); i++ )
		{
			const char* level_id = levels[i]["identifier"].GetString();
			if (strncmp(level_id, "UI_", 3) == 0)
			{
				ParseUILayer(levels[i]);
				continue;
			}

			ECS::Level level;

			// bump the level index
			level.index = level_index++;
			level.id = level_id;

			int level_px_width = levels[i]["pxWid"].GetInt();
			int level_px_height = levels[i]["pxHei"].GetInt();
		
			int level_width = level_px_width / params.gridLength;
			int level_height = level_px_height / params.gridLength;

			int world_offset_x = levels[i]["worldX"].GetInt();
			int world_offset_y = levels[i]["worldY"].GetInt();
			level.worldPos = VectorI(world_offset_x, world_offset_y).toFloat() * level_to_window;
			level.size = VectorI(level_px_width, level_px_height).toFloat() * level_to_window;

			params.level = &level;
			params.levelSize = VectorF(level_width, level_height);

			biome.aabb[0].x = Maths::Min(biome.aabb[0].x, level.worldPos.x);
			biome.aabb[0].y = Maths::Min(biome.aabb[0].y, level.worldPos.y);
			biome.aabb[1].x = Maths::Max(biome.aabb[1].x, level.worldPos.x + level.size.x);
			biome.aabb[1].y = Maths::Max(biome.aabb[1].y, level.worldPos.y + level.size.y);

			Value& layers = levels[i]["layerInstances"];
			for (SizeType i = 0; i < layers.Size(); i++)
			{
				Value& layer = layers[i];
				const char* layer_id = layer["__identifier"].GetString();
				
				if( StringCompare(layer_id, "TerrainColliders" ) )
				{
					ParseTerrainColliders(layer, params);
				}
				else if( StringCompare(layer_id, "Boundaries" ) )
				{
					ParseLayerBoundaries(layer, parser, params);
				}
				// handle all other entities
				else
				{
					ParseEntitiesLayer(layer, level);
				}
			}

			biome.levels.push_back(level);
		}
	}
}