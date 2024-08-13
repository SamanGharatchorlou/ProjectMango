#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Physics.h"
#include "ECS/Components/PlayerController.h"

ECS::Component::Type DebugMenu::DoHealthDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Health;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Health& health = ecs->GetComponentRef(Health, entity);
		ImGui::PushID(entity + (int)type);
		if (ImGui::TreeNode("Component Data"))
		{
			ImGui::Text("Current Health: %.f", health.currentHealth);
			ImGui::Text("Max Health: %.f", health.maxHealth);

			ImGui::Text("Ignored Damage: ");
			for( u32 i = 0; i < health.ignoredDamaged.size(); i++ )
			{
				ImGui::PushID(i);

				ImGui::SameLine();
				ImGui::Text("%d, ", health.ignoredDamaged[i]);

				ImGui::PopID();
			}

			ImGui::Text("Ignored Named Damage: ");
			for( u32 i = 0; i < health.ignoredDamaged.size(); i++ )
			{
				ImGui::PushID(1000 + i);

				if(const char* name = ecs->GetEntityName(health.ignoredDamaged[i]))
					ImGui::Text("%s", name);

				ImGui::PopID();
			}

			ImGui::TreePop();
		}
		ImGui::PopID();
	}

	return type;
}
