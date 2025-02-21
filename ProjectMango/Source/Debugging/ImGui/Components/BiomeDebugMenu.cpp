#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Biome.h"


ECS::Component::Type DebugMenu::DoBiomeDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Biome;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Biome& biome = GetComponentRef(Biome, entity);
		ImGui::PushID(entity + (int)type);

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
						VectorF pos = entity_positions[e].position;
						DebugDraw::Point(pos, Colour::Green);
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

							DebugDraw::RectOutline(rect, Colour::Green);
						}
					}
				}
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	return type;
}
