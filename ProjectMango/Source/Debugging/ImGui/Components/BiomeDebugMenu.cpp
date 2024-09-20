#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Biome.h"


ECS::Component::Type DebugMenu::DoBiomeDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Biome;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Biome& biome = ecs->GetComponentRef(Biome, entity);
		ImGui::PushID(entity + (int)type);

		if(ImGui::TreeNode("Display Entities"))
		{
			for( u32 l = 0; l < biome.levels.size(); l++ )
			{
				const ECS::Level& level = biome.levels[l];
				for( auto iter = level.entities.begin(); iter != level.entities.end(); iter++ )
				{
					const std::vector<VectorF>& entity_positions = iter->second;
					for( u32 e = 0; e < entity_positions.size(); e++ )
					{
						VectorF pos = entity_positions[e];
						DebugDraw::Point(pos, Colour::Green);
					}
				}
			}

			ImGui::TreePop();
		}

		ImGui::PopID();
	}

	return type;
}
