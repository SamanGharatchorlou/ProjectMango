#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/SpacialComponents.h"

u32 DebugMenu::DoBiomeDebugMenu(ECS::Entity& entity)
{
	StringBuffer32 type_name = Biome::TypeName();
	ComponentID type_id = Biome::TypeId();

	if (ImGui::CollapsingHeader(type_name.c_str()))
	{
		ECS::Biome& biome = GetComponentRef(Biome, entity);
		ImGui::PushID(entity + type_id);

		if(ImGui::TreeNode("Display Entities"))
		{
			for( u32 l = 0; l < biome.levels.size(); l++ )
			{
				const ECS::Level& level = biome.levels[l];
				for( auto iter = level.entities.begin(); iter != level.entities.end(); iter++ )
				{
					const std::vector<ECS::EntityMetaData>& entity_positions = iter->second;
					for( u32 e = 0; e < entity_positions.size(); e++ )
					{
						VectorF pos = entity_positions[e].data.GetVector("Position");
						DebugDraw::Point(pos, SColour::Green);
					}
				}
			}

			ImGui::TreePop();
		}

		if(ImGui::TreeNode("Display Walkable Tiles"))
		{
			for( u32 l = 0; l < biome.levels.size(); l++ )
			{
				const ECS::Level& level = biome.levels[l];

				for( u32 y = 0; y < level.walkableTiles.yCount(); y++ )
				{
					for( u32 x = 0; x < level.walkableTiles.xCount(); x++ )
					{
						VectorI index = VectorI(x,y);
						int traversal_value = level.walkableTiles.get( index );
						if(traversal_value == 1)
						{
							
							RectF rect = level.GetWalkableTileRect(index);

							DebugDraw::RectOutline(rect, SColour::Green);
						}
					}
				}
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	return type_id;
}
