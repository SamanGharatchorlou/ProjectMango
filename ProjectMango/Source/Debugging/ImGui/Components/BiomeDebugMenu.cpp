#include "pch.h"
#include "ComponentDebugMenu.h"

#include "imgui-master/imgui.h"
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
			for (u32 i = 0; i < biome.entityMetaData.size(); i++)
			{
				VectorF pos = biome.entityMetaData[i].data.GetVector("Position");
				DebugDraw::Point(pos, SColour::Green);
			}

			ImGui::TreePop();
		}

		if(ImGui::TreeNode("Display Walkable Tiles"))
		{
			for( u32 y = 0; y < biome.walkableTiles.yCount(); y++ )
			{
				for( u32 x = 0; x < biome.walkableTiles.xCount(); x++ )
				{
					VectorI index = VectorI(x,y);
					int traversal_value = biome.walkableTiles.get( index );
					if(traversal_value == 1)
					{
						RectF rect = biome.GetWalkableTileRect(index);
						DebugDraw::RectOutline(rect, SColour::Green);
					}
				}
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	return type_id;
}
