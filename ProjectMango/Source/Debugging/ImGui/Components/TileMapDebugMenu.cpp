#include "pch.h"
#include "ComponentDebugMenu.h"

//#include "ECS/EntityCoordinator.h"
//#include "System/Window.h"
//
//#include "imgui-master/imgui.h"
//#include "Debugging/ImGui/ImGuiHelpers.h"
//#include "ECS/Components/TileMap.h"
//
//static bool s_displayGridLines = true;
//static bool s_displayObjects = true;
//static bool s_aiPathingGrid = true;
//static bool s_showGridIndexes = false;
//static VectorF s_aiGridSize = VectorF(8.0f, 8.0f);
//StringBuffer32 s_selectedMap = "";
//
//ECS::Component::Type DebugMenu::DoTileMapDebugMenu(ECS::Entity& entity)
//{
//	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
//	ECS::Component::Type type = ECS::Component::TileMap;
//
//	ImGui::PushID(entity + (int)type);
//	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
//	{
//        ECS::TileMap& tile_map = ecs->GetComponentRef(TileMap, entity);
//		const SceneTileMapping& map = tile_map.tileMap;
//
//		ImGui::Checkbox("Display Grid Lines", &s_displayGridLines);
//		if (s_displayGridLines && tile_map.tileMap.tileLayers.size() > 0)
//		{
//			VectorF window_size = GameData::Get().window->size();
//			VectorF map_size = map.mapSize;
//			VectorF size_ratio = window_size / map_size;
//
//			const TileLayer& layer = map.tileLayers.front();
//			VectorF tile_size = map.tileSize * size_ratio;
//
//			for (u32 i = 0; i < layer.ids.size(); i++)
//			{
//				VectorI map_index = IndexToMapIndex(i, map.tileCount);
//				VectorF pos = map_index.toFloat() * tile_size;
//				RectF rect(pos, tile_size);
//
//				Colour colour = Colour::Blue;
//
//				if(map_index.x == 0 ||  map_index.x == layer.tileCount.x - 1)
//					colour = Colour::Green;
//				
//				if(map_index.y == 0 ||  map_index.y == layer.tileCount.y - 1)
//					colour = Colour::Green;
//
//				if(map_index.isZero() || map_index == (layer.tileCount - 1)) 
//					colour = Colour::Purple;
//
//				DebugDraw::RectOutline(rect, colour);
//			}
//		}
//
//		ImGui::Checkbox("Display Objects", &s_displayObjects);
//		if (s_displayObjects && tile_map.tileMap.objectLayers.size() > 0)
//		{
//			VectorF window_size = GameData::Get().window->size();
//			VectorF map_size = map.mapSize;
//			VectorF size_ratio = window_size / map_size;
//			VectorF tile_size = map.tileSize * size_ratio;
//
//			for( u32 i = 0; i < map.objectLayers.size(); i++ )
//			{
//				const ObjectLayer& layer = map.objectLayers[i];
//				for( u32 o = 0; o < layer.rects.size(); o++ )
//				{
//					DebugDraw::RectOutline(layer.rects[o], Colour::Purple);
//				}
//			}
//		}
//
//		if(ImGui::Button("Remove Objects"))
//		{
//			VectorF window_size = GameData::Get().window->size();
//			VectorF map_size = map.mapSize;
//			VectorF size_ratio = window_size / map_size;
//			VectorF tile_size = map.tileSize * size_ratio;
//
//			for( u32 i = 0; i < map.colliderEntities.size(); i++ )
//			{
//				GameData::Get().ecs->entities.KillEntity(map.colliderEntities[i]);
//			}
//		}
//
//		ImGui::Checkbox("AI Pathing Grid", &s_aiPathingGrid);
//		ImGui::Checkbox("AI Pathing Grid", &s_aiPathingGrid);
//		ImGui::InputVectorF("Grid Size", s_aiGridSize);
//		if ( s_aiPathingGrid )
//		{
//			VectorF map_size = map.mapSize;
//			VectorF tile_size = s_aiGridSize;
//
//			VectorF window_size = GameData::Get().window->size();
//			VectorF size_ratio = window_size / map_size;
//			VectorF world_tile_size = tile_size * size_ratio;
//
//			VectorI tile_count = (map_size / tile_size).toInt();
//
//			for( u32 x = 0; x < tile_count.x; x++ )
//			{
//				for( u32 y = 0; y < tile_count.y; y++ )
//				{
//					VectorF pos = world_tile_size * VectorI(x,y).toFloat();
//					RectF rect(pos, world_tile_size);
//					DebugDraw::RectOutline(rect, Colour::Green);
//
//					if(s_showGridIndexes)
//					{
//						char buffer[32];
//						_itoa(y * tile_count.x + x, buffer, 10 );
//						DebugDraw::Text(buffer, 10, pos, Colour::White);
//					}
//				}
//			}
//		}
//
//		if (ImGui::BeginCombo("Select Map", s_selectedMap.c_str()))
//		{
//			std::vector<BasicString> maps = FileManager::Get()->allFilesInFolder(FileManager::Maps);
//			for (u32 i = 0; i < maps.size(); i++) 
//			{
//				if(!FileManager::HasExt(maps[i].c_str(), ".xml"))
//					continue;
//
//				StringBuffer64 name = FileManager::Get()->getItemNameAndExt( maps[i].c_str());
//				if (ImGui::Selectable(name.c_str(), false)) 
//				{
//					s_selectedMap = name.c_str();
//				}
//			}
//			ImGui::EndCombo();
//		}
//
//		if(ImGui::ActiveButton("Build Map", !s_selectedMap.empty())) 
//		{
//			SceneBuilder::BuildTileMap(s_selectedMap.c_str(), tile_map.tileMap);
//		}
//	}
//		
//	ImGui::PopID();
//
//	return type;
//}