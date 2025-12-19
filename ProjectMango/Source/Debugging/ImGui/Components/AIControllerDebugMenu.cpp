#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/AIComponents.h"
#include "ECS/EntityCoordinator.h"
#include "Debugging/ImGui/ImGuiHelpers.h"
#include "ECS/Components/Components.h"

u32 DebugMenu::DoAIControllerDebugMenu(ECS::Entity& entity)
{
	ECS::Component::Type type = ECS::Component::AIController;

	ImGui::PushID(entity + (int)type);
	if (ImGui::CollapsingHeader(ECS::ComponentNames[type]))
	{
		ECS::AIController& aic = GetComponentRef(AIController, entity);
		
		Entity target = Target::GetValidTarget(entity);

		const VectorF distance = GetPosition(entity) - GetPosition(target);
		const float target_distance = distance.length();

		ImGui::Text("Has target: %s", ECS::GetName(target));
		ImGui::Text("Distance to target %f", target_distance);

		//ImGui::Text("Can move to target: %s", aic.canMoveToTarget ? "true" : "false");
	}
		
	ImGui::PopID();

	return (u32)type;
}

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