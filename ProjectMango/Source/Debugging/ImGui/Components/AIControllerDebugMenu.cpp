#include "pch.h"
#include "ComponentDebugMenu.h"

#include "ECS/Components/AIController.h"
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

		ImGui::Text("Has target: %s", ECS::GetName(aic.target));
		ImGui::Text("Distance to target %f", aic.VectorToTarget().length());

		ImGui::Text("Can move to target: %s", aic.canMoveToTarget ? "true" : "false");
	}
		
	ImGui::PopID();

	return (u32)type;
}