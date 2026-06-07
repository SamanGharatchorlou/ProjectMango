#include "pch.h"
#include "SceneReader.h"

#include "ECS/Components/IncludeComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Entities/Factory/EntitySerialiser.h"
#include "Graphics/TextureManager.h"
#include "System/Window.h"
#include "UI/UIManager.h"
#include "Entities/Factory/EntityBuilder.h"

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

	struct Params
	{
		ECS::Biome* biome = nullptr;

		std::unordered_map<int, BasicString> valueDefines;

		VectorF levelToWindow;
		VectorF levelSize;
		int gridLength = 0;
	};

	static void ParseEntitiesLayer(Value& layer, ECS::Biome& biome)
	{
		const Value::Array& entities = layer["entityInstances"].GetArray();
		for (u32 e = 0; e < entities.Size(); e++)
		{
			Value& entry = entities[e];

			ECS::EntityMetaData emd;
			PopulateMetaDataFromJson(entry, emd);

			biome.entityMetaData.push_back(emd);
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
				u32 index = ent_y * (u32)params.levelSize.x + ent_x;
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
						int x_index = ent_y * (int)params.levelSize.x + x_idx;
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
							int y_index = y_idx * (int)params.levelSize.x + xy_ent;
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

		params.biome->walkableTiles.set(params.levelSize.toInt(), 0);// = Grid<int>(params.levelSize.x, params.levelSize.y, 0);
		for (u32 ent_x = 0; ent_x < params.levelSize.x; ent_x++)
		{
			for (u32 ent_y = 0; ent_y < params.levelSize.y; ent_y++)
			{
				int index = ent_y * (int)params.levelSize.x + ent_x;
				int value = entities[index].GetInt();
				params.biome->walkableTiles.get(ent_x, ent_y) = value;
			}
		}
	}

	static void ParseLayerBoundaries(Value& layer, JSONParser& parser, const Params& params)
	{
		ECS::Layer new_layer;
		params.biome->layers.push_back(new_layer);
		ECS::Layer& level_layer = params.biome->layers.back();

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

			Value& src = entry["src"];
			float src_x = src[0].GetFloat();
			float src_y = src[1].GetFloat();

			ECS::Layer::Tile tile;
			tile.draw_pos = VectorF(px_x, px_y);
			tile.tileset_pos = VectorF(src_x, src_y);
			level_layer.tiles.push_back(tile);
		}
	}

	static void AddSideBar(RectF rect)
	{
		Entity entity = CreateEntity("side bar");
		Transform& transform = AddComponent(Transform, entity);
		transform.SetWorldPosition(rect.TopLeft());
		transform.size = rect.Size();

		Sprite& sprite = AddComponent(Sprite, entity);
		sprite.SetTexture("EditorBg_black");
		sprite.params.renderLayer = RenderLayer::Bottom;
	}

	static void ParseBiome(const char* biome_id, ECS::Biome& biome)
	{
		BasicString file;
		FileManager::Get()->FindFile(FileManager::Maps, biome_id, file);

		JSONParser parser(file.c_str());
		if (!parser.IsValid())
			return;

		const VectorF window_size = GameData::Get().window->size();

		// base screen level width is 256, i.e. thats the non-streched size of the screen
		const float level_to_window_x = window_size.x / parser.document["defaultLevelWidth"].GetFloat();;
		const float level_to_window_y = window_size.y / parser.document["defaultLevelHeight"].GetFloat();
		VectorF level_to_window(level_to_window_x, level_to_window_y);
		GameData::Get().window->windowToLevel = VectorF(1.0f / level_to_window_x, 1.0f / level_to_window_y);

		Params params;
		params.gridLength = parser.document["defaultGridSize"].GetInt();
		params.levelToWindow = level_to_window;
		params.biome = &biome;

		Value& defines = parser.document["defs"];
		Value::Array layers = defines["layers"].GetArray();
		for (u32 i = 0; i < layers.Size(); i++)
		{
			if (StringCompare(layers[i]["identifier"].GetString(), "TerrainColliders"))
			{
				const Value::Array& grid_values = layers[i]["intGridValues"].GetArray();
				for (u32 i = 0; i < grid_values.Size(); i++)
				{
					Value& grid_value = grid_values[i];
					params.valueDefines[grid_value["value"].GetInt()] = grid_value["identifier"].GetString();
				}
			}
		}

		const Value::Array& levels = parser.document["levels"].GetArray();
		for (u32 i = 0; i < levels.Size(); i++)
		{
			if (i != biome.biomeIndex)
				continue;

			// bump the level index
			biome.id = levels[i]["identifier"].GetString();

			int level_px_width = levels[i]["pxWid"].GetInt();
			int level_px_height = levels[i]["pxHei"].GetInt();

			int level_width = level_px_width / params.gridLength;
			int level_height = level_px_height / params.gridLength;

			int world_offset_x = levels[i]["worldX"].GetInt();
			int world_offset_y = levels[i]["worldY"].GetInt();
			biome.size = VectorI(level_px_width, level_px_height).toFloat() * level_to_window;

			//params.level = &level;
			params.levelSize = VectorF(level_width, level_height);

			Value& layers = levels[i]["layerInstances"];
			for (SizeType i = 0; i < layers.Size(); i++)
			{
				Value& layer = layers[i];
				const char* layer_id = layer["__identifier"].GetString();

				if (StringCompare(layer_id, "TerrainColliders"))
				{
					ParseTerrainColliders(layer, params);
				}
				else if (StringCompare(layer_id, "Boundaries"))
				{
					ParseLayerBoundaries(layer, parser, params);
				}
				// handle all other entities
				else
				{
					ParseEntitiesLayer(layer, biome);
				}
			}
		}

		// add black bars around the edge of the map
		// tbh this was better before when it was handled by LDtk, as i could set the colour...
		const float dim = 60.0f;
		{
			VectorF size(dim, biome.size.y + 2 * dim);
			// left
			{
				VectorF position(-dim, -dim);
				AddSideBar(RectF(position, size));
			}
			// right
			{
				VectorF position(biome.size.x, -dim);
				AddSideBar(RectF(position, size));
			}
		}
		{
			VectorF size(biome.size.x + 2 * dim,  dim);
			// top
			{
				VectorF position(-dim, -dim);
				AddSideBar(RectF(position, size));
			}
			// bot
			{
				VectorF position(-dim, biome.size.y);
				AddSideBar(RectF(position, size));
			}
		}
	}

	ECS::Entity BuildBiome(const char* biome_id, int biome_index, int level_index)
	{
		ECS::Entity biome_entity = ECS::CreateEntity("Scene");
		ECS::Biome& biome = AddComponent(Biome, biome_entity);
		biome.biomeIndex = biome_index;
		biome.levelIndex = level_index;

		ParseBiome(biome_id, biome);

		return biome_entity;
	}

	ECS::Entity BuildBiomeAndEntities(const char* biome_id, int biome_index, int level_index)
	{
		ECS::Entity entity = BuildBiome(biome_id, biome_index, level_index);
		BuildBiomeEntities(entity);

		return entity;
	}
}