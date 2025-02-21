#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Physics.h"

ECS::Component::Type DebugMenu::DoHealthDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Health;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Health& health = GetComponentRef(Health, entity);
		ImGui::PushID(entity + (int)type);

		ImGui::Text("Current Health: %.f", health.currentHealth);
		ImGui::Text("Max Health: %.f", health.maxHealth);

		ImGui::PopID();
	}

	return type;
}


ECS::Component::Type DebugMenu::DoEntityDataDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::EntityData;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::EntityData& entity_data = GetComponentRef(EntityData, entity);
		ImGui::PushID(entity + (int)type);

		ECS::EntityManager& em = ecs->entities;
		const char* parent = entity_data.parent != ECS::EntityInvalid ? ECS::GetName(entity) : "No parent";
		ImGui::Text("Parent: %s", parent);

		for( u32 i = 0; i < entity_data.children.size(); i++ )
		{
			const char* child = ECS::GetName(entity_data.children[i]);
			ImGui::Text("Child: %s", child);
		}

		ImGui::PopID();
	}

	return type;
}