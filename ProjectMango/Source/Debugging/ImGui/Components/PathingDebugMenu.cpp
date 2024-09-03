#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"

ECS::Component::Type DebugMenu::DoPathingDebugMenu(ECS::Entity& entity)
{
	ECS::EntityCoordinator* ecs = GameData::Get().ecs;
	ECS::Component::Type type = ECS::Component::Pathing;

	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Pathing& pathing = ecs->GetComponentRef(Pathing, entity);
		ImGui::PushID(entity + (int)type);


	
		ImGui::PopID();
	}

	return type;
}