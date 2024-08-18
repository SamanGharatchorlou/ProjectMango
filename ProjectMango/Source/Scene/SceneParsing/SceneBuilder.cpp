#include "pch.h"
#include "SceneBuilder.h"

#include "Graphics/TextureManager.h"
#include "System/Files/FileManager.h"
#include "System/Files/ConfigManager.h"
#include "System/Window.h"

#include "ECS/EntityCoordinator.h"
#include "ECS/Components/Components.h"
#include "ECS/Components/Collider.h"
#include "Core/Helpers.h"

using namespace Map;

static void PopulateTileSet(XMLNode& tileset_node, TileSet& tile_set)
{
	StringMap32 attributes;
	attributes.fillAtributes(tileset_node);

	tile_set.startingIndex = attributes.getInt("firstgid");

	const char* tileset_source = attributes.getString("source");
	BasicString tileset_config = FileManager::Get()->findFileEtx(FileManager::Maps, tileset_source);
	
	XMLParser* tilseset_parser = new XMLParser;
	tilseset_parser->parseXML(tileset_config.c_str());

	StringMap32 tilset_attr;
	tilset_attr.fillAtributes(tilseset_parser->rootNode());
	tilset_attr.fillAtributes(tilseset_parser->rootChild("image"));

	tile_set.texture = TextureManager::Get()->getTexture(tilset_attr.getString("source"), FileManager::Maps);
	tile_set.mapSize = tilset_attr.getVectorF("width", "height");
	tile_set.tileSize = tilset_attr.getVectorF("tilewidth", "tileheight");

	VectorF tile_count = (tile_set.mapSize / tile_set.tileSize) + VectorF(0.5f, 0.5f);
	tile_set.tileCount = tile_count.toInt();

	delete tilseset_parser;
}

static void PopulateLayerData(TileLayer& layer, XMLNode layer_node)
{
	StringMap32 attributes;
	attributes.fillAtributes(layer_node);

	layer.name = attributes.getString("name");
	layer.tileCount = attributes.getVectorI("width", "height");
	// todo: render layer

	const char* data_string = layer_node.child("data").value();

	u32 value = 0;
	bool is_value = false;
	while (*data_string != '\0')
	{
		// we dont know what we might find so make sure it just valid numbers
		if (*data_string < '0' || *data_string > '9')
		{
			data_string++;

			if (is_value)
			{
				layer.ids.push_back(value);
			}

			is_value = false;
			value = 0;
			continue;
		}

		is_value = true;
		value = value * 10 + (*data_string++ - '0');
	}
}

static RectF BuildObjectRect(XMLNode object_node, VectorF size_ratio)
{
	StringMap32 objectAttributes;
	objectAttributes.fillAtributes(object_node);

	// create the rect
	VectorF pos = objectAttributes.getVectorF("x", "y");
	VectorF size = objectAttributes.getVectorF("width", "height");
	RectF rect(pos, size);
	rect.Scale(size_ratio);

	return rect;
}

static void PopulateObjectLayerData(const XMLNode& object_layer_node, SceneTileMapping& tile_mapping, VectorF size_ratio)
{
	StringMap32 attributes;
	attributes.fillAtributes(object_layer_node);

	tile_mapping.objectLayers.push_back(ObjectLayer());
	ObjectLayer& object_layer = tile_mapping.objectLayers.back();
	object_layer.id = attributes.getInt("id");
	object_layer.name = attributes.at("name");

	XMLNode object_node = object_layer_node.child("object");
	while (object_node)
	{
		// create the rect
		RectF rect = BuildObjectRect(object_node, size_ratio);
		object_layer.rects.push_back(rect);

		// create a static collider entity
		ECS::EntityCoordinator* ecs = GameData::Get().ecs;
		ECS::Entity ent = ecs->CreateNewEntity();

		ECS::Collider& collider = ecs->AddComponent(Collider, ent);
		collider.SetRect( rect );
		SetFlag<u32>(collider.mFlags, (u32)ECS::Collider::Static);

		ECS::Transform& transform = ecs->AddComponent(Transform, ent);
		//transform.rect = rect;

		tile_mapping.colliderEntities.push_back(ent);

		// next object
		object_node = object_node.next();
	}
}

void SceneBuilder::BuildTileMap(const char* mapName, SceneTileMapping& tile_mapping)
{
	FileManager* fm = FileManager::Get();
	BasicString full_path = FileManager::Get()->findFileEtx(FileManager::Maps, mapName);

	if (full_path.empty())
		return;

	XMLParser* parser = new XMLParser;
	parser->parseXML(full_path.c_str());

	// Fill Base Map Data
	const XMLNode map_node = parser->rootNode();
	StringMap32 map_attributes;
	map_attributes.fillAtributes(map_node);

	tile_mapping.tileCount = map_attributes.getVectorI("width", "height");
	tile_mapping.tileSize = map_attributes.getVectorF("tilewidth", "tileheight");
	tile_mapping.mapSize = tile_mapping.tileCount.toFloat() * tile_mapping.tileSize;

	const VectorF window_size = GameData::Get().window->size();
	const VectorF size_ratio = window_size / tile_mapping.mapSize;

	// Fill Layer Data
	XMLNode layer_node = map_node.child("layer");
	while (layer_node)
	{
		tile_mapping.tileLayers.push_back(TileLayer());
		PopulateLayerData(tile_mapping.tileLayers.back(), layer_node);
		layer_node = layer_node.next("layer");
	}

	// Populate Tile Set Data
	XMLNode tileset_node = map_node.child("tileset");
	while (tileset_node)
	{
		tile_mapping.tilesets.push_back(TileSet());
		PopulateTileSet(tileset_node, tile_mapping.tilesets.back());

		tileset_node = tileset_node.next("tileset");
	}

	std::sort(tile_mapping.tilesets.begin(), tile_mapping.tilesets.end(),
		[](TileSet& a, TileSet& b) { return a.startingIndex > b.startingIndex; });

	// Populate Object Data
	XMLNode object_layer_node = map_node.child("objectgroup");
	while (object_layer_node)
	{
		if(StringCompare(object_layer_node.attribute("name")->value(), "PlayerSpawn"))
		{
			if(XMLNode object_node = object_layer_node.child("object"))
			{
				tile_mapping.playerSpawnArea = BuildObjectRect(object_node, size_ratio);;
			}
		}
		else if(StringCompare(object_layer_node.attribute("name")->value(), "EnemySpawn"))
		{
			if(XMLNode object_node = object_layer_node.child("object"))
			{
				tile_mapping.enemySpawnArea = BuildObjectRect(object_node, size_ratio);;
			}
		}
		else
		{
			//PopulateObjectLayerData(object_layer_node, tile_mapping, size_ratio);
		}

		object_layer_node = object_layer_node.next("objectgroup");
	}

	delete parser;
}
