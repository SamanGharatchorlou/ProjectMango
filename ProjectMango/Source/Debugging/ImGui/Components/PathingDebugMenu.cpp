#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Components.h"

u32 DebugMenu::DoPathingDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::Pathing;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::Pathing& pathing = GetComponentRef(Pathing, entity);

		ImGui::Text("Has valid path: ", GetBoolString(pathing.hasValidPath));
	}
	ImGui::PopID();

	return (u32)type;
}